//
// Created by 14685 on 2022/7/3.
//

#include "Transmission.h"
#include "QuaternionAHRS.h"
#include "main.h"
#include "usbd_cdc_if.h"
#include "Gimbal.h"
#include "usart.h"
#include "bsp_can.h"

BCPFrameTypeDef upper_rx_data;
BCPFrameTypeDef upper_tx_all_data[FRAME_NUM];
BCPFrameTypeDef upper_tx_data;
BCPRpyTypeDef rpy_rx_data;
BCPRpyTypeDef rpy_tx_data;

BCPGimStateTypeDef gim_state_data;   // ��̨״̬����

float testdata[4]={0};
RecvFrameTypeDef auto_rx_data;
//��ʼ�����鷢��֡
SendFrameTypeDef auto_tx_data =
        {
                .head=0xbbbb,
                .rotateDirection=1,
                .index=0
//                .index=0,
//                .q0=0.0f,
//                .q1=0.0f,
//                .q2=0.0f,
//                .q3=0.0f,
        };

/*�ϰ���°巢�͵�����*/
static uint8_t cm_data[8] = {0};
extern volatile float yaw_angle_ref_v;

extern uint8_t USB_SEND_OK;

// ������Щʲô��
//  - cm_data: ���͸����̵� �Ƕ�����
//  - auto_tx_data: ���͸���λ�� ��������� ��Ŀǰֻ����pitch��yaw�Ƕ��Լ���̨����ģʽ�Լ���ת����
//  - rpy_tx_data: ͨ��usb���͸� ��λ���� ŷ��������
//  - testdata: ��������

// xxx_data �� xxx_buffer ��ʲô��ϵ��
//  - xxx_data �ǰ�����֡ͷ��֡β������������������֡data��xxx_buffer�ǵ��������ݲ��֡��ڽ������Ĵ����м���֡ͷ��֡β����� xxx_data

// ��Щ�Ƕȶ���ʲô��
//  - xxx_angle_fdb: �����Ƕȣ���λ��degree�� ��imu������ֵ��ȥoffset(�������ʱ��imu�Ƕȣ��õ�
//  - xxx_relative_angle����ԣ�����ֵ���Ƕȣ���λ��degree�����õ����������ֵ��д���Ĺ�����ɵ�ֵ������ģ�
//  - xxx_angle_ref�������Ƕȣ���λ��degree
//  - gim.xxx_offset_angle���������ʱimu�ĽǶ�ֵ��һ��ʼʹ�� xxx_relative_angle ��Ϊ���ڷ����Ƕȣ�xxx_angle_fdb)���ڵõ����ֵ֮���������ˣ�imu����ȷ��
//  - ͬ���������v����ָ���ٶ�

void transmission_task(void const * argument)
{
    int8_t rpy_tx_buffer[FRAME_RPY_LEN] = {0} ;
    int32_t rpy_data = 0;
    uint32_t *gimbal_rpy = (uint32_t *)&rpy_data;

    int8_t gim_state_tx_buffer[GIMBAL_STATE_LEN] = {0} ;

    int8_t imu_tx_buffer[FRAME_IMU_LEN] = {0} ;
    int32_t imu_data = 0;
    uint32_t *gimbal_imu = (uint32_t *)&imu_data;

    __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
    uint32_t transmission_wake_time = osKernelSysTick();

    // �����Ƿ������ݣ���̨���������� `usbd_cdc_if.c` �е�**�ص�����** CDC_Receive_FS() ����д���
    while (1)
    {
        /*���°巢������*/
        Get_Communicate_data(cm_data, CAN_RPY_TX);
        Send_Communicate_data(&COM_CAN, cm_data, CAN_RPY_TX);

        auto_tx_data.pitchAngleGet=pit_angle_fdb;
        auto_tx_data.yawAngleGet=yaw_angle_fdb;
        auto_tx_data.gimbal_mode=gim.ctrl_mode;
        testdata[0]=-AHRS.Pitch;
        testdata[1]=-AHRS.Roll;
        testdata[2]=-AHRS.Yaw;

// imu.angle_x - gim.yaw_offset_angle , ���� yaw_angle_fdb��pitchͬ��
        /* USB���ͽǶ�֡ */
        rpy_tx_buffer[0] = 0;
        rpy_data = (imu.angle_x - gim.yaw_offset_angle) * 1000;
        rpy_tx_buffer[1] = *gimbal_rpy;
        rpy_tx_buffer[2] = *gimbal_rpy >> 8;
        rpy_tx_buffer[3] = *gimbal_rpy >> 16;
        rpy_tx_buffer[4] = *gimbal_rpy >> 24;
        rpy_data = (imu.angle_y - gim.pit_offset_angle) * 1000;
        rpy_tx_buffer[5] = *gimbal_rpy;
        rpy_tx_buffer[6] = *gimbal_rpy >> 8;
        rpy_tx_buffer[7] = *gimbal_rpy >> 16;
        rpy_tx_buffer[8] = *gimbal_rpy >> 24;
        rpy_data = imu.angle_z * 1000;
        rpy_tx_buffer[9] = *gimbal_rpy;
        rpy_tx_buffer[10] = *gimbal_rpy >> 8;
        rpy_tx_buffer[11] = *gimbal_rpy >> 16;
        rpy_tx_buffer[12] = *gimbal_rpy >> 24;

        /* ��̨״̬����֡ */
        /* ��3λ��ʱû���ϣ����ձ��� */
        gim_state_tx_buffer[0] = gim.ctrl_mode;
        gim_state_tx_buffer[1] = 0;
        gim_state_tx_buffer[2] = 0;
        gim_state_tx_buffer[3] = 0;

        Add_Frame_To_Upper(GIMBAL, rpy_tx_buffer);
        CDC_Transmit_FS((uint8_t*)&rpy_tx_data, sizeof(rpy_tx_data));

        Add_Frame_To_Upper(GIMBAL_STATE, gim_state_tx_buffer);
        CDC_Transmit_FS((uint8_t*)&gim_state_data, sizeof(gim_state_data));

        osDelayUntil(&transmission_wake_time, 1);
    }

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    //���Ӳ��ͬ��
    if(GPIO_Pin==CAM_IO_Pin)
    {
        CDC_Transmit_FS((uint8_t*)&auto_tx_data,sizeof(auto_tx_data));
        int i=(auto_tx_data.index%25)*2;
        //��¼������һ֡ʱ��̨��λ��
        angle_history[i]=yaw_angle_fdb;
        angle_history[i+1]=pit_angle_fdb;
        auto_tx_data.index++;
        HAL_GPIO_TogglePin(GPIOH,CAM_IO_Pin);
    }
}

void Get_Communicate_data(uint8_t* data, uint16_t send_type){
    switch (send_type) {
        case CAN_RPY_TX: {
            //c��֧�ָ�������λ��������Ϊ������������������治ͬ
            //���ｫ������ת��Ϊ������������λ(����ǿ������ת�����ǽ��������ռ���ڴ�Ľ��͹���ת��Ϊ����
            uint32_t *yaw_angle = (uint32_t *)&yaw_relative_angle;
            uint32_t *yaw_ref_v = (uint32_t *)&yaw_angle_ref_v;
            //��һλ�����ж��Ƿ��ѵ���
            data[3] = *yaw_angle >> 24;
            data[2] = *yaw_angle >> 16;
            data[1] = *yaw_angle >> 8;
            data[0] = *yaw_angle;
            data[7] = *yaw_ref_v >> 24;
            data[6] = *yaw_ref_v >> 16;
            data[5] = *yaw_ref_v >> 8;
            data[4] = *yaw_ref_v;
        }break;
        default: break;
    }
}

void Send_Communicate_data(CAN_HandleTypeDef *_hcan, uint8_t *data, uint16_t send_type) {
    static CAN_TxHeaderTypeDef TX_MSG;
    static uint8_t CAN_Send_Data[8];
    uint32_t send_mail_box;

    TX_MSG.StdId = CAN_UP_TX_INFO;
    TX_MSG.IDE = CAN_ID_EXT;            /* ���°�ͨѶʹ����չ֡ */
    TX_MSG.ExtId = send_type;
    TX_MSG.RTR = CAN_RTR_DATA;
    TX_MSG.DLC = 0x08;
    TX_MSG.TransmitGlobalTime = DISABLE;
    CAN_Send_Data[0] = data[0];
    CAN_Send_Data[1] = data[1];
    CAN_Send_Data[2] = data[2];
    CAN_Send_Data[3] = data[3];
    CAN_Send_Data[4] = data[4];
    CAN_Send_Data[5] = data[5];
    CAN_Send_Data[6] = data[6];
    CAN_Send_Data[7] = data[7];
    HAL_CAN_AddTxMessage(_hcan, &TX_MSG, CAN_Send_Data, &send_mail_box);
}

void Add_Frame_To_Upper(uint16_t send_mode, int8_t* data_buf){
    switch (send_mode) {
        case GIMBAL:{
            rpy_tx_data.HEAD = 0XFF;
            rpy_tx_data.D_ADDR = MAINFLOD;
            rpy_tx_data.ID = GIMBAL;
            rpy_tx_data.LEN = FRAME_RPY_LEN;
            memcpy(&rpy_tx_data.DATA, data_buf, sizeof(rpy_tx_data.DATA));

            /* �� odom ֡��ת�浽��ת֡��������У����� */
            memcpy(&upper_tx_data, &rpy_tx_data, sizeof(rpy_tx_data));
            rpy_tx_data.SC = (uint8_t)Sumcheck_Cal(upper_tx_data) >> 8;
            rpy_tx_data.AC = (uint8_t)Sumcheck_Cal(upper_tx_data);
            memset(&upper_tx_data, 0, sizeof(upper_tx_data));
        }break;
        default:break;
    }
}

//TODO: У��������
uint16_t Sumcheck_Cal(BCPFrameTypeDef frame){
    uint8_t sumcheck = 0;
    uint8_t addcheck = 0;
    uint16_t allcheck = 0;

    sumcheck += frame.HEAD;
    addcheck += sumcheck;
    sumcheck += frame.D_ADDR;
    addcheck += sumcheck;
    sumcheck += frame.ID;
    addcheck += sumcheck;
    sumcheck += frame.LEN;
    addcheck += sumcheck;

    for(int i = 0; i<frame.LEN; i++){
        sumcheck += frame.DATA[i];
        addcheck += sumcheck;
    }
    allcheck = (uint16_t)(sumcheck << 8 | addcheck);
    return allcheck;
}
