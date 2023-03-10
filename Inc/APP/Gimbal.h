//
// Created by 14685 on 2022/7/3.
//

#ifndef HNU_RM_UP_GIMBAL_H
#define HNU_RM_UP_GIMBAL_H

#include <user_lib.h>
#include "stm32f4xx_hal.h"
#include "bsp_uart.h"
#include "stdint.h"
#include "stdint.h"
#include "Ins.h"

/* ��̨�������� (ms) */
#define GIMBAL_PERIOD 1
/* ��̨���г�ʼ��ʱ�� (ms) */
#define BACK_CENTER_TIME 6000

#define YAW_MOTOR_MAXOUT 30000
#define PITCH_MOTOR_MAXOUT 30000

#define YAW_V_PID_MAXOUT_M_INIT 15000
#define YAW_V_PID_MAXOUT_M 25000
#define YAW_V_PID_MAXINTEGRAL_M 3000
#define YAW_V_PID_KP_M 500
#define YAW_V_PID_KI_M 20.0
#define YAW_V_PID_KD_M 10.0
#define YAW_V_PID_LPF_M 0.000001
#define YAW_V_PID_D_LPF_M 0.005

#define YAW_A_PID_MAXOUT_M 320
#define YAW_A_PID_MAXINTEGRAL_M 320
#define YAW_A_PID_KP_M 30
#define YAW_A_PID_KI_M 0.5
#define YAW_A_PID_KD_M 0.001

#define YAW_V_PID_MAXOUT_A 25000
#define YAW_V_PID_MAXINTEGRAL_A 3000
#define YAW_V_PID_KP_A 500
#define YAW_V_PID_KI_A 20.0
#define YAW_V_PID_KD_A 10.0
#define YAW_V_PID_LPF_A 0.000001
#define YAW_V_PID_D_LPF_A 0.005

#define YAW_A_PID_MAXOUT_A 320
#define YAW_A_PID_MAXINTEGRAL_A 320
#define YAW_A_PID_KP_A 30
#define YAW_A_PID_KI_A 200
#define YAW_A_PID_KD_A 0.001

#define YAW_V_FFC_MAXOUT_INIT 0
#define YAW_V_FFC_MAXOUT 0
#define YAW_V_FCC_C0 1000
#define YAW_V_FCC_C1 20
#define YAW_V_FCC_C2 0
#define YAW_V_FCC_LPF 0.005

#define YAW_A_FFC_MAXOUT 0
#define YAW_A_FCC_C0 0
#define YAW_A_FCC_C1 0.005
#define YAW_A_FCC_C2 0
#define YAW_A_FCC_LPF 0.0001


#define PITCH_V_PID_MAXOUT_A 16000
#define PITCH_V_PID_MAXINTEGRAL_A 2000
#define PITCH_V_PID_KP_A 16
#define PITCH_V_PID_KI_A 2.0
#define PITCH_V_PID_KD_A 0.005
#define PITCH_V_PID_LPF_A 0.000001
#define PITCH_V_PID_D_LPF_A 0.08

#define PITCH_A_PID_MAXOUT_A 3000
#define PITCH_A_PID_MAXINTEGRAL_A 600
#define PITCH_A_PID_KP_A 90.0
#define PITCH_A_PID_KI_A 180
#define PITCH_A_PID_KD_A 1.0
#define PITCH_A_PID_LPF_A 0.1
#define PITCH_A_PID_D_LPF_A 0.01

#define PITCH_V_FFC_MAXOUT_A 30000
#define PITCH_V_FCC_C0_A 25
#define PITCH_V_FCC_C1_A 100 * 0
#define PITCH_V_FCC_C2_A 0
#define PITCH_V_FCC_LPF_A 0.005

#define PITCH_A_FFC_MAXOUT_A 320
#define PITCH_A_FCC_C0_A 0
#define PITCH_A_FCC_C1_A 1
#define PITCH_A_FCC_C2_A 0
#define PITCH_A_FCC_LPF_A 0.00001

#define PITCH_V_PID_MAXOUT_INIT_M 8000
#define PITCH_V_PID_MAXOUT_M 12000
#define PITCH_V_PID_MAXINTEGRAL_M 3000
#define PITCH_V_PID_KP_M 16
#define PITCH_V_PID_KI_M 0
#define PITCH_V_PID_KD_M 0.015
#define PITCH_V_PID_LPF_M 0.000001
#define PITCH_V_PID_D_LPF_M 0.08

#define PITCH_A_PID_MAXOUT_M 320
#define PITCH_A_PID_MAXINTEGRAL_M 600
#define PITCH_A_PID_KP_M 90.0
#define PITCH_A_PID_KI_M 0
#define PITCH_A_PID_KD_M 3.5
#define PITCH_A_PID_LPF_M 0.1
#define PITCH_A_PID_D_LPF_M 0.01

#define PITCH_V_FFC_MAXOUT_M 30000
#define PITCH_V_FCC_C0_M 25
//#define PITCH_V_FCC_C0_M 0
//#define PITCH_V_FCC_C1_M 100 *_M 0
#define PITCH_V_FCC_C1_M 0
//#define PITCH_V_FCC_C2_M 0
#define PITCH_V_FCC_C2_M 0
#define PITCH_V_FCC_LPF_M 0.005

#define PITCH_A_FFC_MAXOUT_M 320
#define PITCH_A_FCC_C0_M 1
#define PITCH_A_FCC_C1_M 1
//#define PITCH_A_FCC_C1_M 0
#define PITCH_A_FCC_C2_M 0
#define PITCH_A_FCC_LPF_M 0.00001

/**
  * @brief     ��̨����ģʽö��
  */
typedef enum
{
    GIMBAL_INIT = 0,         //��̨��ʼ��
    GIMBAL_RELAX = 1,            //��̨�ϵ�
    GIMBAL_CLOSE_LOOP_ZGYRO = 2, //��̨����imu z��Ƕ�
    GIMBAL_AUTO	= 3						 //��̨����ģʽ
} GimbalModeType;

/**
  * @brief     ��̨�����ź�����״̬ö��
  */
typedef enum
{
    NO_ACTION = 0,           //�޿����ź�����
    IS_ACTION = 1,               //�п����ź�����
} ActionModeType;

/**
  * @brief     ��̨����״̬ö��
  */
typedef enum
{
    PIT_BACK_STEP,           //��̨ pitch �����
    YAW_BACK_STEP,           //��̨ yaw �����
    BACK_IS_OK,              //��̨�������
} GimbalBackType;

/**
  * @brief     ��̨�������ݽṹ��
  */
typedef struct
{
    GimbalModeType ctrl_mode; //��̨��ǰ����ģʽ
    GimbalModeType last_mode; //��̨�ϴο���ģʽ

    ActionModeType ac_mode;   //��̨�����ź�����ģʽ

    uint8_t  no_action_flag; //�޿����źű�־
    uint32_t no_action_time; //�޿����ź�ʱ��

    float ecd_offset_angle;  //��̨��ʼ������ֵ
    float yaw_offset_angle;  //��̨��ʼ yaw ��Ƕ� ����imu�ã�
    float pit_offset_angle;  //��̨��ʼ pit ��Ƕ� ����imu�ã�
} GimbalYawTypeDef;





/**
  * @brief     ��̨����������
  */
void gimbal_task(const void* argu);

/**
  * @brief     ��̨���Ʋ�����ʼ��
  */
static void Gimbal_Init_param(void);

/**
  * @brief     ��ȡ��̨��������������Ϣ
  */
static void Gimbal_Get_information(void);

/**
  * @brief     ��ȡ��̨����ģʽ�������ģʽ�ж�����̨���г�ʼ����������
  * @retval    GIMBAL_RELAX or GIMBAL_INIT
  */
static void Gimbal_Get_mode(void);

/**
  * @brief     ��̨���г�ʼ��ģʽ������
  */
static void Gimbal_Init_handle(void);

/**
  * @brief     ��̨����������ջ����ƴ�����
  */
static void Gimbal_Loop_handle(void);

/**
  * @brief     ��̨������ƴ���ӿ�
  */
void Gimbal_Auto_control(void);

/**
  * @brief     ��̨relaxģʽ������
  */
static void Gimbal_Relax_handle(void);

/**
  * @brief     ��̨ yaw ��λ�ñջ�����
  * @retval    yaw refer angle, unit is degree.
  */
void Gimbal_Control_yaw(void);

/**
  * @brief     ��̨ pitch ��λ�ñջ�����
  * @retval    pitch refer angle, unit is degree.
  */
void Gimbal_Control_pitch(void);

/**
  * @brief     ��̨�ջ����Ƶ�������͵���
  */
void Gimbal_Control_moto(void);

/**
  * @brief     get relative position angle to center
  * @param[in] raw_ecd: gimbal motor encoder raw angle
  * @param[in] center_offset: read gimbal_cali_data from chip flash
  * @retval    relative angle, unit is degree.
  */
static int16_t Gimbal_Get_relative_pos(int16_t raw_ecd, int16_t center_offset);

/**
  * @brief     ������̨��ʼ������ʱ����ز���
  */
static void Gimbal_Back_param(void);

/**
  * @brief     ����̨��PID�����л�Ϊ�ֶ�ģʽ
  */
void PID_Reset_manual(void);

/**
  * @brief     ����̨��PID�����л�Ϊ�Զ�ģʽ
  */
void PID_Reset_auto(void);



extern GimbalYawTypeDef gim;

/* ��̨��ԣ�����ڹ���ֵ���Ƕ�,unit: degree ���ɵ���������ã�*/
extern volatile float yaw_relative_angle;
extern float pit_relative_angle;

/* ��̨ PID ����������� */
extern float yaw_angle_ref;
extern float pit_angle_ref;
extern float yaw_angle_fdb;
extern float pit_angle_fdb;
extern ImuTypeDef imu;    //����IMU��������ص�����

extern bool_t recv_flag;    //���⴮�ڽ��ձ�־λ
extern float angle_history[50];    //���25֡��ʷ��̬����



#endif //HNU_RM_UP_GIMBAL_H