/*
 * mtu3.c
 *
 *  Created on: 2022/03/01
 *      Author: assistmotion01
 */
#include <machine.h>
#include <math.h>
#include "iodefine.h"
#include "cmt.h"
#include "bldc_test.h"
#include "sns.h"
#include "mtu3.h"

#define SOHO_PWM_PERIOD	(2400)							/* 96MHz�~50us */
#define DEAD_TIME		(192)							/* 96MHz�~2us */
#define BLDC_ROUND_ENC	(1200.0f)
#define BLDC_2PI      	(6.2831853f)                 	/* 2�� */
#define BLDC_05PI		(3.14159265f/2)
#define BLDC_SQRT_2_3	(0.81649658f)
#define BLDC_QSI        (0.006198f)                     /* �i�v���Γd�@�q��������(�����l) */
#define BLDC_RESIST     (0.453f)                        /* �d�@�q��R */
#define BLDC_LD         (0.0009447f)                    /* d���C���_�N�^���X */
#define BLDC_LQ         (0.0009447f)                    /* q���C���_�N�^���X */
#define DELTA_TIME		(0.0001f)

#define LPF_K_OMEGA		(0.1)

#if SIM
/* #define LPF_K_CURRENT	(1) */
#else
#define LPF_K_CURRENT	(0.1)
#endif

/* �d��PI���� */
#define     KP_ID      	(3)
#define     KI_ID       (0.0005)
#define     KP_IQ       (3)
#define     KI_IQ       (0.0005)
#define		BLDC_VD_LIMIT (11.0f)
#define		BLDC_VQ_LIMIT (11.0f)
#define		VDC			(24.0f)

/* ���xPI���� */
#define		SPEED_PI_CNT	(10)


#define		RPM_RAD_COF					((2*3.14159265f)/60)

/* �O���[�o���ϐ� */
/* �d���l */
static float gIu;
static float gIu_Pre;
static float gIw;
static float gIw_Pre;
static float gIv;
static float gId;
static float gIq;

/*�n�����d���Z���T������*/
static float gAdjust_Iu;
static float gAdjust_Iw;
static int gAdjust_Current_Cnt;

/* �d��PI���� */
static float gVdIsum;
static float gVqIsum;
static float gVd;
static float gVq;
static float gBldc_Angle;
static float gBldc_Speed;

/* ���xPI���� */
static int gSpdPICnt;
static float gbldc_round_enc;
static float gpre_encd;
static float glpf_omega;

float gVdc;
float gRpm;

int gDebugCnt;
int gLEDon;
/* �֐��v���g�^�C�v�錾 */
/* �����݊֐� */
#pragma interrupt (bldc_interrupt(vect=138))

static void bldc_interrupt(void);
//static void clear_mot_interrupt(void);

void initialize_mtu34(void)
{
	/* ���[�^�[�p�^�C�}������ */
    MTU.TSTRA.BYTE = 0x00;                              /* MTU0~4�^�C�}��~ */

    MTU3.TCR.BIT.TPSC = 0;                              /* �������Ȃ� : ICLK(96MHz) */
    MTU3.TCR.BIT.CKEG = 0;								/* �����G�b�W�ŃJ�E���g�@*/
    MTU4.TCR.BIT.TPSC = 0;                              /* �������Ȃ� : ICLK(96MHz) */
    MTU4.TCR.BIT.CKEG = 0;								/* �����G�b�W�ŃJ�E���g�@*/

    MTU.TGCRA.BIT.BDC = 0;								/* �ʏ�o�́@*/

	gDebugCnt = 0;
	gLEDon = 0;
}

void initialize_pwm(void)
{
    /* PWM�ݒ� */
    MTU3.TCNT = DEAD_TIME;                     		    /* HW�}�j���A���}16.39:[4]�f�b�h�^�C���ݒ� */
    MTU4.TCNT = 0;										/* HW�}�j���A���}16.39:0000h�ݒ�@*/

    MTU3.TGRB = (SOHO_PWM_PERIOD + DEAD_TIME)/2;          /* HW�}�j���A���}16.39:[6]PWM�f���[�e�B�ݒ�@*/
    MTU4.TGRA = (SOHO_PWM_PERIOD + DEAD_TIME)/2;
    MTU4.TGRB = (SOHO_PWM_PERIOD + DEAD_TIME)/2;
    MTU3.TGRD = (SOHO_PWM_PERIOD + DEAD_TIME)/2;          /* HW�}�j���A���}16.39:[6]PWM�f���[�e�B�ݒ�(�o�b�t�@��)�@*/
    MTU4.TGRC = (SOHO_PWM_PERIOD + DEAD_TIME)/2;
    MTU4.TGRD = (SOHO_PWM_PERIOD + DEAD_TIME)/2;

    MTU.TDDRA = DEAD_TIME;								/* HW�}�j���A���}16.39:[8]�f�b�h�^�C���E�L�����A�����̐ݒ�@*/
    MTU.TCDRA = SOHO_PWM_PERIOD;
    MTU.TCBRA = SOHO_PWM_PERIOD;
    MTU3.TGRA = SOHO_PWM_PERIOD + DEAD_TIME;                      /* MTU3��TGRA��TGRC�ɂ̓L�����A����/2+�f�b�h�^�C����ݒ� */
    MTU3.TGRC = SOHO_PWM_PERIOD + DEAD_TIME;

    MTU.TOCR1A.BIT.PSYE = 0;							/* HW�}�j���A���}16.39:[9]�g�O���o�͂����Ȃ� */
    MTU.TOCR1A.BIT.OLSP = 1;                            /* ����PWM���o�̓��x���I��(�|�W��) : �����lLow,�A�b�v��High�A�_�E����Low */
    MTU.TOCR1A.BIT.OLSN = 1;                            /* ����PWM���o�̓��x���I��(�l�K��) : �����lLow,�A�b�v��Low�A�_�E����High */

    MTU3.TMDR1.BIT.MD = 14;                             /* HW�}�j���A���}16.39:[10]����PWM���[�h�I��(�J�œ]��) */
    MTU3.TMDR1.BIT.BFA = 1;                             /* TGRA��TGRC�̓o�b�t�@���� */
    MTU3.TMDR1.BIT.BFB = 1;                             /* TGRB��TGRD�̓o�b�t�@���� */
    MTU4.TMDR1.BYTE = 0x00;								/* MTU3�𑊕�PWM�Ƃ�������MTU4���ݒ�͖����A�����l�ݒ� */

    MTU.TOERA.BYTE = 0xC0;                              /* PWM�o�͒�~�őҋ@  */
}

void initialize_mot_interrupt(void)
{
    /* ���荞�ݐݒ� */
    MTU4.TSR.BIT.TCFV = 0;                              /* TCNT�A���_�[�t���[�������ݗv������HW�}�j���A���F16.3.1(b) */
    MTU4.TIER.BIT.TCIEV = 1;							/* �I�[�o�[�t���[��������Ă��邪TCNT��0xFFFF�𒴂���O�Ƀ_�E�����Ă� */
    ICU.IPR[0x5A].BIT.IPR = 14;							/* �؂�ւ�邽�ߎ����A���_�[�t���[���̂� */
    ICU.IR[138].BIT.IR = 0;
    ICU.IER[0x11].BIT.IEN2 = 1;

    //MTU.TITCR1A.BIT.T4VEN = 0;							/* HW�}�j���A���}16.77:[1]���荞�݊Ԉ����J�E���^�N���A */
    MTU.TITMRA.BIT.TITM = 0;                            /* HW�}�j���A���}16.77:[2]TITCR1�Őݒ�  */
    MTU.TITCR1A.BIT.T4VCOR = 1;							/* HW�}�j���A���}16.77:[3]�Ԉ�����1�� */
    MTU.TITCR1A.BIT.T4VEN = 1;
    MTU.TSTRA.BYTE = 0xC0;
}

void initialize_encoder(void)
{
    MTU1.TMDR1.BIT.MD = 4;								/* �ʑ��W�����[�h1(MTU1�A2�ŃJ�E���g)�@*/
    MTU1.TCR.BIT.CCLR = 0;								/* �G���R�[�_�̓\�t�g�Ǘ�(�N���A���Ȃ�)*/
    MTU.TSTRA.BIT.CST1 = 1;								/* �G���R�[�_�Ď��J�n */
    /* �|�[�g�ݒ��port.c */
}

void reset_mtu3(void)
{
	gIu 					= 0;
	gIu_Pre 				= 0;
	gIw						= 0;
	gIw_Pre					= 0;
	gIv						= 0;
	gId						= 0;
	gIq						= 0;
	gAdjust_Iu				= 0;
	gAdjust_Iw				= 0;
	gAdjust_Current_Cnt		= 0;
	gVdIsum					= 0;
	gVqIsum					= 0;
	gVd						= 0;
	gVq						= 0;
	gBldc_Angle				= 0;
	gBldc_Speed				= 0;
	gSpdPICnt				= 0;
  	gbldc_round_enc 		= 0;
    gpre_encd 				= 0;
    glpf_omega				= 0;
}

void start_pwm(void)
{
//	   MTU.TSTRA.BYTE = 0xC0;                           /* HW�}�j���A���}16.77:[13]CST3�A4�𓯎��X�^�[�g */
	   MTU.TOERA.BYTE = 0xFF;                           /* PWM output enable */
}

void stop_pwm(void)
{
//	MTU.TSTRA.BYTE = 0x00;                           /* MTU0~4�^�C�}��~ */
	MTU.TOERA.BYTE = 0xC0;                           /* PWM�o�͒�~ */
}

void clear_mot_interrupt(void)
{
    MTU4.TSR.BIT.TCFV = 0;                              /* �A���_�[�t���[(�J����)�t���O�N���A */
    ICU.IR[138].BIT.IR = 0;								/* TCFV�ɊY�����銄���ݗv���N���A */
}


void set_motpwm_uvw(float pwm_u, float pwm_v, float pwm_w, float vdc)
{
    float carrier_half_period, vdc_n;

    carrier_half_period = (float)((SOHO_PWM_PERIOD + DEAD_TIME)/2);
    vdc_n = (vdc / 2);

    pwm_u = -pwm_u;
    pwm_v = -pwm_v;
    pwm_w = -pwm_w;

    MTU3.TGRD = (unsigned short)(((pwm_u * carrier_half_period) / vdc_n) + carrier_half_period);
    MTU4.TGRC = (unsigned short)(((pwm_v * carrier_half_period) / vdc_n) + carrier_half_period);
    MTU4.TGRD = (unsigned short)(((pwm_w * carrier_half_period) / vdc_n) + carrier_half_period);
}

float get_encd_pos(void)
{
    float encorder;

    encorder = (float)(MTU1.TCNT);

    return (encorder);
}

void set_encd_pos(float set_pos)
{
	unsigned short base_pos = (unsigned short)set_pos;
    MTU1.TCNT = base_pos;
    gbldc_round_enc = base_pos;
    gpre_encd = base_pos;
}

void get_mot_speed(float *prad, float *pomega)
{
    float encd;
    float delta_enc;
    float mot_rad;
    float delta_rad;
    float omega;
    short idelta_enc;

    encd = get_encd_pos();

    idelta_enc = (short)(gpre_encd - encd);
    delta_enc = (float)idelta_enc;
    gbldc_round_enc += delta_enc;
	if (gbldc_round_enc >= BLDC_ROUND_ENC) {
		gbldc_round_enc -= BLDC_ROUND_ENC;
	}
	else if (gbldc_round_enc < 0) {
		gbldc_round_enc += BLDC_ROUND_ENC;
	}
		
    if (get_bldc_drive_state() >= DRIVE){
        /* radian */
        mot_rad = ((BLDC_2PI * gbldc_round_enc) * BLDC_MGPOLE_PAIR / BLDC_ROUND_ENC);      /* angle: 0 to 7*2pi [rad] */

        /* speed */
        delta_rad = ((BLDC_2PI * delta_enc) * BLDC_MGPOLE_PAIR / BLDC_ROUND_ENC);
        omega = (delta_rad / DELTA_TIME);
        glpf_omega += LPF_K_OMEGA * (omega- glpf_omega);

        *prad = mot_rad;
		*pomega = glpf_omega;
		//tprintf("%f, %f, %f, %f, %f\n", mot_rad,delta_rad,glpf_omega,encd,spre_encd);
    }
    else
    {
    	//tprintf("rad, delta_rad, omega, encd, pre_encd\n");
        /* speed */
        *pomega = 0;

        if (get_bldc_drive_state() <= ADJUST1){
            *prad = 0;
        }
        else if (get_bldc_drive_state() >= START2){
            /* angle */
            *prad = BLDC_05PI;
        	//*prad = 0;
        }
    }

    gpre_encd = encd;
}

void dq_conversion(float iu, float iv, float iw, float angle)
{
	float id_u, id_v, id_w;
	float iq_u, iq_v, iq_w;

    id_u = cosf(angle) * iu;
    id_v = cosf(angle - (BLDC_2PI / 3)) * iv;
    id_w = cosf(angle + (BLDC_2PI / 3)) * iw;
    gId = BLDC_SQRT_2_3 * (id_u + id_v + id_w);

    iq_u = sinf(angle) * iu;
    iq_v = sinf(angle - (BLDC_2PI / 3)) * iv;
    iq_w = sinf(angle + (BLDC_2PI / 3)) * iw;
    gIq = (-1) * BLDC_SQRT_2_3 * (iq_u + iq_v + iq_w);
}

void dq_reconversion(float vd_ref, float vq_ref, float angle, float *pvu, float *pvv, float *pvw)
{
	float vref_u, vref_v, vref_w;
	float vref_limit = VDC/2;
	/* dq�t�ϊ� */
	vref_u = BLDC_SQRT_2_3 * (vd_ref * cosf(angle) - vq_ref * sinf(angle) );
	vref_v = BLDC_SQRT_2_3 * (vd_ref * cosf(angle - BLDC_2PI/3 ) - vq_ref * sinf(angle - BLDC_2PI/3 ) );
	vref_w = BLDC_SQRT_2_3 * (vd_ref * cosf(angle + BLDC_2PI/3 ) - vq_ref * sinf(angle + BLDC_2PI/3 ) );
	/* �e���d������ */
    /* U�� */
	if ( vref_u > vref_limit){
        vref_u = vref_limit;
    }
    else if (vref_u < -vref_limit){
    	vref_u = -vref_limit;
    }
    else{
    }

	 /* V�� */
	if ( vref_v > vref_limit){
        vref_v = vref_limit;
    }
    else if (vref_v < -vref_limit){
    	vref_v = -vref_limit;
    }
    else{
    }

	 /* W�� */
	if ( vref_w > vref_limit){
        vref_w = vref_limit;
    }
    else if (vref_w < -vref_limit){
    	vref_w = -vref_limit;
    }
    else{
    }

	*pvu = vref_u;
	*pvv = vref_v;
	*pvw = vref_w;
}

void get_dq_current(float *pid, float *piq)
{
	*pid = gId;
	*piq = gIq;
}

float currenr_pi_ctrl(float current_ref, float current, float kp, float ki, float limit, float *psum)
{
	/* ------------- vd�o��---------------- */
	float vref, vref_p, vref_i;
	float idiff;

	idiff = current_ref - current;

	/* P�� */
 	vref_p = kp * idiff;

 	/* I�� */
 	vref_i = *psum;
	vref_i = vref_i + ki * idiff;

	if(vref_i > limit){
		vref_i = limit;
	}
	else if(vref_i < (-limit) ){
		vref_i = -limit;
	}
	*psum = vref_i;

	/* PI�o�� */
	vref = vref_p + vref_i;
	if(vref > limit){
		vref = limit;
	}
	else if(vref < (-limit) ){
		vref = -limit;
	}
	return vref;
}

void bldc_current_interrupt(void)
{
	float vdc;
	float id, iq, id_ref, iq_ref;
	float vd, vq, decp_vd, decp_vq;
	float vu, vv, vw;
	float bldc_refspeed;

	/* �d���A�d���擾 */
	clrpsw_i();
	get_mot_current(&gIu, &gIw, &vdc);
	setpsw_i();

	gVdc = vdc;

	/* �n�����d������ */
	if(gAdjust_Current_Cnt <= CURRENT_ADJ_CNT){
		gAdjust_Iu += LPF_K_CURRENT * (gIu - gAdjust_Iu);
		gIu = -gIu + gAdjust_Iu;
		//gIu = gIu + gAdjust_Iu;
		gAdjust_Iw += LPF_K_CURRENT * (gIw - gAdjust_Iw);
		gIw = -gIw + gAdjust_Iw;
		//gIw = gIw + gAdjust_Iw;

        gAdjust_Current_Cnt++;
	}
	else{
		/* �쓮���d�����o */
		gIu = -gIu + gAdjust_Iu;
		gIw = -gIw + gAdjust_Iw;
		//gIu = gIu + gAdjust_Iu;
		//gIw = gIw + gAdjust_Iw;

		gIu = gIu_Pre + LPF_K_CURRENT * (gIu - gIu_Pre);
		gIu_Pre = gIu;
		gIw = gIw_Pre + LPF_K_CURRENT * (gIw - gIw_Pre);
		gIw_Pre = gIw;

	}
	gIv = - (gIu + gIw);

	/* ���x�擾 */
	get_mot_speed(&gBldc_Angle, &gBldc_Speed);

	/* �G���[�`�F�b�N */
	check_bldc_error(gIu, gIv, gIw, vdc, gBldc_Speed);

	gRpm = gBldc_Speed/RPM_RAD_COF/7;
	/* dq�ϊ� */
	dq_conversion(gIu, gIv, gIw, gBldc_Angle);
	get_dq_current(&id, &iq);
	get_dq_ref(&id_ref, &iq_ref);
	//tprintf("id_ref=%f, iq_ref=%f\n",id_ref, iq_ref);

	gId = id;
	gIq = iq;
	/* �d��PI���� */
	vd = currenr_pi_ctrl(id_ref, id, KP_ID, KI_ID, BLDC_VD_LIMIT, &gVdIsum);
	vq = currenr_pi_ctrl(iq_ref, iq, KP_IQ, KI_IQ, BLDC_VQ_LIMIT, &gVqIsum);

	/* �񊱏��� */

	bldc_refspeed = get_bldc_refspeed();
	decp_vd = bldc_refspeed * BLDC_LQ * iq;
	decp_vq = bldc_refspeed * (BLDC_LD * id + BLDC_QSI);
	//decp_vd = 0;
	//decp_vq = 0;
	vd = vd - decp_vd;
	vq = vq + decp_vq;

	/* dq�t�ϊ� */

	dq_reconversion(vd, vq, gBldc_Angle, &vu, &vv, &vw);
	/* pwm_duty�ݒ� */
	//tprintf("%f, %f, %f, %f\n", vu,vv,vw,vdc);
	//if(gAdjust_Current_Cnt <= CURRENT_ADJ_CNT){
	//	vu = 0;
	//	vv = 0;
	//	vw = 0;
	//}
	set_motpwm_uvw(vu, vv, vw, vdc);
}

void bldc_interrupt(void)
{
	bldc_current_interrupt();
	
	gSpdPICnt++;
	if( gSpdPICnt >= SPEED_PI_CNT){
		bldc_speed_interrupt();
		gSpdPICnt = 0;
	}
	
	clear_mot_interrupt();
}

float get_bldc_angle(void)
{
	return gBldc_Angle;
}

float get_bldc_speed(void)
{
	return gBldc_Speed;
}

int get_current_adjust_cnt(void)
{
	return gAdjust_Current_Cnt;
}

