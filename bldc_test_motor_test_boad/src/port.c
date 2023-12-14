/*
 * port.c
 *
 *  Created on: 2022/03/02
 *      Author: assistmotion01
 */
#include "iodefine.h"

void initialize_port(void)
{
	/* PWM�o�͒[�q */
    PORT7.DDR.BIT.B1 = 1;                               /* U��H�o�͐ݒ� */
    PORT7.DDR.BIT.B2 = 1;                               /* V��H�o�͐ݒ� */
    PORT7.DDR.BIT.B3 = 1;                               /* W��H�o�͐ݒ� */
    PORT7.DDR.BIT.B4 = 1;                               /* U��L�o�͐ݒ� */
    PORT7.DDR.BIT.B5 = 1;                               /* V��L�o�͐ݒ� */
    PORT7.DDR.BIT.B6 = 1;   
                               							 /* W��L�o�͐ݒ� */
	PORT7.DR.BIT.B1 = 0;                             /* HW�}�j���A���}(�\15.27)P71/MTIOC3B(Up) = "L" */
	PORT7.DR.BIT.B4 = 0;                             /* HW�}�j���A���}(�\15.27)P74/MTIOC3D(Un) = "L" */
	PORT7.DR.BIT.B2 = 0;                             /* HW�}�j���A���}(�\15.27)P72 MTIOC4A(Vp) = "L" */
	PORT7.DR.BIT.B5 = 0;                             /* HW�}�j���A���}(�\15.27)P75/MTIOC4C(Vn) = "L" */
	PORT7.DR.BIT.B3 = 0;                             /* HW�}�j���A���}(�\15.27)P73/MTIOC4B(Wp) = "L" */
	PORT7.DR.BIT.B6 = 0;    
	
    /* �G���R�[�_�[�q */
    PORT3.ICR.BIT.B3 = 1;                               /* A���̓��̓o�b�t�@�L�� */
    PORT3.DDR.BIT.B3 = 0;								/* �f�[�^���͕��� */
    PORT3.ICR.BIT.B2 = 1;                               /* B���̓��̓o�b�t�@�L�� */
    PORT3.DDR.BIT.B2 = 0;								/* �f�[�^���͕��� */

    PORT4.ICR.BIT.B0 = 1;                               /* the input buffer is enabled for ADC */
    PORT4.ICR.BIT.B1 = 1;
    PORT4.ICR.BIT.B2 = 1;
    PORT4.ICR.BIT.B6 = 1;

    /* �d���A�d���Z���T */
    S12AD0.ADCSR.BIT.TRGE = 0;                          /* ADSTRGR�ݒ��AD�ϊ� */
    S12AD0.ADCSR.BIT.CKS = 3;							/* ���ӃN���b�N�g�p(48MHz)*/
    S12AD0.ADCSR.BIT.ADIE = 0;							/* �ϊ���̊����݂Ȃ� */
    S12AD0.ADCSR.BIT.ADCS = 1;							/* 1�T�C�N���X�L�������[�h */
    S12AD0.ADANS.BIT.CH = 2;							/* AN000~AN002���X�L���� */
    S12AD0.ADCER.BIT.SHBYP = 0;							/* �T���v���z�[���h��H�g�p */
    S12AD0.ADCER.BIT.ADRFMT = 0;						/* �f�[�^�t�H�[�}�b�g�E�l�� */
    //S12AD0.ADSTRGR.BIT.ADSTRS0 = 5;						/* MTU4.TCNT�̃A���_�[�t���[�ŕϊ� */

    /* ���x����p�ϒ�R�Z���T */
    S12AD1.ADCSR.BIT.TRGE = 0;                          /*ADSTRGR�ݒ��AD�ϊ� */
    S12AD1.ADCSR.BIT.CKS = 3;							/* ���ӃN���b�N�g�p(48MHz)*/
    S12AD1.ADCSR.BIT.ADIE = 0;							/* �ϊ���̊����݂Ȃ� */
    S12AD1.ADCSR.BIT.ADCS = 0;							/* �V���O�����[�h */
    S12AD1.ADANS.BIT.CH = 2;							/* AN102�`�����l���I�� */
    S12AD1.ADCER.BIT.ADRFMT = 0;						/* �f�[�^�t�H�[�}�b�g�E�l�� */
    //S12AD1.ADSTRGR.BIT.ADSTRS0 = 5;						/* MTU4.TCNT�̃A���_�[�t���[�ŕϊ� */

}

void reset_port(void)
{
	PORT7.DR.BIT.B1 = 0;                             /* HW�}�j���A���}(�\15.27)P71/MTIOC3B(Up) = "L" */
	PORT7.DR.BIT.B4 = 0;                             /* HW�}�j���A���}(�\15.27)P74/MTIOC3D(Un) = "L" */
	PORT7.DR.BIT.B2 = 0;                             /* HW�}�j���A���}(�\15.27)P72 MTIOC4A(Vp) = "L" */
	PORT7.DR.BIT.B5 = 0;                             /* HW�}�j���A���}(�\15.27)P75/MTIOC4C(Vn) = "L" */
	PORT7.DR.BIT.B3 = 0;                             /* HW�}�j���A���}(�\15.27)P73/MTIOC4B(Wp) = "L" */
	PORT7.DR.BIT.B6 = 0;                             /* HW�}�j���A���}(�\15.27)P76/MTIOC4D(Wn) = "L" */
}

void initialize_ui(void)
{
    /*************** SW1,2 ***************/
    PORT9.ICR.BIT.B1 = 1;							 /* SW�|�[�g���̓o�b�t�@�L�� */
    PORT9.ICR.BIT.B2 = 1;
    PORT9.DDR.BIT.B1 = 0;							 /* SW�|�[�g����͐ݒ� */
    PORT9.DDR.BIT.B2 = 0;
    /*************** LED1,2 ***************/
    PORTA.DR.BIT.B2 = 1;							/* LED�|�[�g���� */
    PORTA.DR.BIT.B3 = 1;
    PORTA.DDR.BIT.B2 = 1;							/* LED�|�[�g�o�͐ݒ� */
    PORTA.DDR.BIT.B3 = 1;
}
