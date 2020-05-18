/*
 * File:   main.c
 * Author: YH31
 * Created on April 29, 2020, 8:38 AM
 * 
 * ��ʾ������
 * RC2��ΪPWM�������
 * ���PWM������Ϊ51.4us
 * ��һ������ռ�ձ�Ϊ25%
 * �ڶ�������ռ�ձ�Ϊ50%
 * ����������ռ�ձ�Ϊ75%
 * �ظ����������������
 */


#include <xc.h>
/**
 * CCP1 ģ��� PWM ������ʽ��ʼ��
 * CCPģ��:capture/compare/PWM ������/�Ƚ�/PWM��
 * CCP����һ��16λ�Ĵ���CCPx������Ϊ���¼Ĵ���
    * 16λ��capture�Ĵ���
    * 16λ��compare�Ĵ���
    * PWM����ռ�ձȼĴ���
 * 
    * pic16F877A��������ccpģ�� CCP1,CCP2
 *  ����ģ�������������һ���ģ�������CCP1Ϊ��
 * CCP1�ļĴ���CCPR1������8λ�Ĵ������
 * CCPR1L(��8λ)
 * CCPR1H(��8λ)
 * ����һ���Ĵ�������CCP1
 * CCP1CON
 * 
 *CCPʹ�õĶ�ʱ����Դ
 * captrueʹ�õ���Timer1
 * compareʹ�õ���Timer1
 * PWMʹ�õ���Timer2
 * */
int count=0;
void PWMInit(){
    //����PWM�Ĺ�������
    //Tosc=1/��Ƶ��;
    //�������ڼ��㹫ʽT=(PR2+1)*4*Tosc*��T2CON<1:0>����ʾ��Ԥ��ֵ��
    //T=(PR2+1)*4*1/20*��T2CON<1:0>����ʾ��Ԥ��ֵ��
    //��ΪPWM�õ�����Timer2 8λ�Ĵ���
    //timer2��00��ʼ������������PR2��ȣ����������timer2
    //PR2����Ϊ0xFFʱ��һ������Ϊ51.4us
    PR2=0xFF;//
    //����ռ�ձ�
    //ռ�ձȼ��㹫ʽ��
    //t2��ʾ����T2CON<1:0>����ʾ��Ԥ��ֵ��
    //PWM Duty cycle=(CCPR1L:CCP1CON<5:4>)*1/20*t2
    //�����CCPR1L:CCP1CON<5:4>��ʾ����һ��10λ�Ķ�������K��Ҳ��ʾ���PWM�ķֱ���Ϊ10λ
    //CCPR1L������K�ĸ߰�λ��CCP1COn<5:4>������K�ĵͶ�λ
    //Ҳ����˵���Ĺ�ʽΪPDC(ռ�ձ�)=K*1/20*t2
    //�������Ҫռ�ձ�Ϊ25%��Ԥ��ֵΪ1
    //��ôPDC/T=0.5
    //(K*1/20*1)/51.4=0.5
    //K=ռ�ձ�*51.4*20
    //������K=257,
    //CCPR1L=K�ĸ�8λ=0x40
    //CCP1CON<5:4>=1:0
    CCPR1L=0x40;
    //ʹ��PWMģʽ,������CCP1CON<5:4>Ϊ0��1
    CCP1CON=_CCP1CON_CCP1Y_MASK|_CCP1CON_CCP1M2_MASK|_CCP1CON_CCP1M3_MASK;
    //��CCP1��������Ϊ���ģʽ
    TRISC2=0;
    //����timer2Ԥ��Ƶ����ʹ��timer2
    //Ԥ��Ƶ1
    T2CON=0x04;
    
}

/**
 * ����timer2�ж�ʹ��
 */
void Timer2IEInit(){
    //���������ж�,�Լ��ⲿ�ж�
    INTCON=_INTCON_GIE_MASK|_INTCON_PEIE_MASK;
    //ʹ��timer2�ж�
    //ʹ��PIE1ǰ��������INTCON��PEIEλ
    PIE1=_PIE1_TMR2IE_MASK;    
}
/**
 * ͨ��Timer2�ж�������ռ�ձ�
 * 
 */
void PWMModulate(){
    //��һ������ռ�ձ�Ϊ25% ,k=257 ,CCPR1L=0x40, 
    //�ڶ�������ռ�ձ�Ϊ50%��k=514
    //����������ռ�ձ�Ϊ75%��k=771
//    CCP1CONbits.CCP1X=0;
//    CCP1CONbits.CCP1Y=0;
    if(count==4){
        //��count==4��ʱ�������һ����count=1,
        count=1;
         CCPR1L=0x40,CCP1CONbits.CCP1X=0,CCP1CONbits.CCP1Y=1;
        return;
    }
    switch(count){
        case 1:
            CCPR1L=0x40,CCP1CONbits.CCP1X=0,CCP1CONbits.CCP1Y=1;
            break;
        case 2:
            CCPR1L=0x80,CCP1CONbits.CCP1X=1,CCP1CONbits.CCP1Y=0; ;
            break;
        case 3:
             CCPR1L=0xC0,CCP1CONbits.CCP1X=1,CCP1CONbits.CCP1Y=1;
            
            ;break;
    }
    
}

//�жϺ���,PWM��һ�����ڽ���һ���ж�
//51.4us����һ���ж�

void __interrupt() PWM(void){
    //�ж��Ƿ���timer2�ж�
    if(PIE1bits.TMR2IE&&PIR1bits.TMR2IF){
        //��λ��־λ
        PIR1bits.TMR2IF=0;
        count++;
        PWMModulate();
    }
}
void main(void) {
    
    PWMInit();
    //��Timer2�ж�
    Timer2IEInit();
   
    while(1){
        
    };
    return;
}
