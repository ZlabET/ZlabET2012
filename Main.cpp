#include "LightSensor.h"
#include "GyroSensor.h"
#include "SonarSensor.h"
#include "TouchSensor.h"
#include "BlueTooth.h"
#include "Motor.h"
#include "Tail.h"
#include "Input.h"
#include "UI.h"
#include "PID.h"
#include "balanceControl.h"
#include "Control.h"
#include "SlopeDetection.h"
#include "ColorDetection.h"
#include "ObstacleDetection.h"
#include "Detection.h"
#include "Section.h"

extern "C"{
	#include "kernel.h"
	#include "kernel_id.h"
	#include "ecrobot_interface.h"
};

void ecrobot_device_initialize(){
	// �Z���T�A���[�^�Ȃǂ̊e�f�o�C�X�̏������֐���
	// �����Ŏ������邱�Ƃ��ł��܂�
	// �ˁ@���Z���T�ԐFLED��ON�ɂ���
	ecrobot_set_light_sensor_active(NXT_PORT_S1);
	// �ˁ@�����g�Z���T(I2C�ʐM)��������
	ecrobot_init_sonar_sensor(NXT_PORT_S3);
	
	if(ecrobot_get_bt_status() == BT_NO_INIT){
		ecrobot_set_bt_device_name("ET337");
	}
	
	ecrobot_init_bt_slave("unagipai");
}
	
void ecrobot_device_terminate(){
	// �Z���T�A���[�^�Ȃǂ̊e�f�o�C�X�̏I���֐���
	// �����Ŏ������邱�Ƃ��ł��܂��B
	// �ˁ@���Z���T�ԐFLED��OFF�ɂ���
	ecrobot_set_light_sensor_inactive(NXT_PORT_S1);
	// �ˁ@�����g�Z���T(I2C�ʐM)���I��
	ecrobot_term_sonar_sensor(NXT_PORT_S3 );
	
	ecrobot_term_bt_connection();
}
	
	// 1msec�������荞��(ISR�J�e�S��2)����N������郆�[�U�[�p�t�b�N�֐�
void user_1ms_isr_type2(void){
	// ����͉����s��Ȃ�
}

extern "C" TASK(TaskMain){
	LightSensor *lightSensor = new LightSensor(NXT_PORT_S1);
	GyroSensor *gyroSensor = new GyroSensor(NXT_PORT_S2);
	SonarSensor *sonarSensor = new SonarSensor(NXT_PORT_S3);
	TouchSensor *touchSensor = new TouchSensor(NXT_PORT_S4);
	Motor *rightMotor = new Motor(NXT_PORT_A);
	Tail *tail = new Tail(NXT_PORT_B);
	Motor *leftMotor = new Motor(NXT_PORT_C);
	BlueTooth *blueTooth = new BlueTooth();
	UI *ui = new UI(*touchSensor, *blueTooth, *lightSensor, *tail);
	Input *input = new Input(*gyroSensor, *lightSensor, *sonarSensor);
	PID *pid = new PID();
	BalanceControl *balanceControl = new BalanceControl();
	Control *control = new Control(*pid, *balanceControl, *leftMotor, *rightMotor, *tail);
	SlopeDetection *slopeDetection = new SlopeDetection();
	ColorDetection *colorDetection = new ColorDetection();
	ObstacleDetection *obstacleDetection = new ObstacleDetection();
	
	int course = ui->courseSelect();
		
	if(course == 1){
		// ��ԑJ�ڏ����̐ݒ�
		//                                        gyro_threshold,    g, light_threshold,    l,    s, count
		Detection *basicDetection = new Detection(             0,   -1,               0,   -1,   -1,   500, *slopeDetection, *colorDetection, *obstacleDetection);
		// ��ԃp�����[�^�ݒ�
		//                           forward,forward_var, turn,tail,tail_var,gyro_offset,pid_flag, pid_target,   KP,balance_flag,balance_init
		Section *basic = new Section(    100,          0,    0,  90,       0,          0,    TRUE,        600, -0.7,       FALSE,       FALSE, *input, *control, *basicDetection);
		
		ui->waitStart(100);
		
		basic->basicRun();
		
	}
	else{
		while(1){
			
		}
	}
}
