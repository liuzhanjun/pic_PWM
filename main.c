/*
 * File:   main.c
 * Author: YH31
 * Created on April 29, 2020, 8:38 AM
 * 
 * 本示例功能
 * RC2作为PWM输出引脚
 * 输出PWM的周期为51.4us
 * 第一个周期占空比为25%
 * 第二个周期占空比为50%
 * 第三个周期占空比为75%
 * 重复输出上面三个周期
 */


#include <xc.h>
/**
 * CCP1 模块的 PWM 工作方式初始化
 * CCP模块:capture/compare/PWM （捕获/比较/PWM）
 * CCP包含一个16位寄存器CCPx可以作为如下寄存器
    * 16位的capture寄存器
    * 16位的compare寄存器
    * PWM主从占空比寄存器
 * 
    * pic16F877A包含两个ccp模块 CCP1,CCP2
 *  两个模块操作基本上是一样的，下面以CCP1为例
 * CCP1的寄存器CCPR1由两个8位寄存器组成
 * CCPR1L(低8位)
 * CCPR1H(高8位)
 * 另有一个寄存器控制CCP1
 * CCP1CON
 * 
 *CCP使用的定时器资源
 * captrue使用的是Timer1
 * compare使用的是Timer1
 * PWM使用的是Timer2
 * */
int count=0;
void PWMInit(){
    //设置PWM的工作周期
    //Tosc=1/震荡频率;
    //工作周期计算公式T=(PR2+1)*4*Tosc*（T2CON<1:0>所表示的预定值）
    //T=(PR2+1)*4*1/20*（T2CON<1:0>所表示的预定值）
    //因为PWM用到的是Timer2 8位寄存器
    //timer2从00开始自增，自增到PR2相等，则溢出重置timer2
    //PR2设置为0xFF时，一个周期为51.4us
    PR2=0xFF;//
    //设置占空比
    //占空比计算公式：
    //t2表示：（T2CON<1:0>所表示的预定值）
    //PWM Duty cycle=(CCPR1L:CCP1CON<5:4>)*1/20*t2
    //上面的CCPR1L:CCP1CON<5:4>表示的是一个10位的二进制数K，也表示这个PWM的分辨率为10位
    //CCPR1L代表着K的高八位，CCP1COn<5:4>代表着K的低二位
    //也就是说最后的公式为PDC(占空比)=K*1/20*t2
    //如果我想要占空比为25%，预定值为1
    //那么PDC/T=0.5
    //(K*1/20*1)/51.4=0.5
    //K=占空比*51.4*20
    //算出结果K=257,
    //CCPR1L=K的高8位=0x40
    //CCP1CON<5:4>=1:0
    CCPR1L=0x40;
    //使用PWM模式,并设置CCP1CON<5:4>为0：1
    CCP1CON=_CCP1CON_CCP1Y_MASK|_CCP1CON_CCP1M2_MASK|_CCP1CON_CCP1M3_MASK;
    //将CCP1引脚设置为输出模式
    TRISC2=0;
    //设置timer2预分频，并使能timer2
    //预分频1
    T2CON=0x04;
    
}

/**
 * 配置timer2中断使能
 */
void Timer2IEInit(){
    //允许所有中断,以及外部中断
    INTCON=_INTCON_GIE_MASK|_INTCON_PEIE_MASK;
    //使能timer2中断
    //使用PIE1前必须配置INTCON的PEIE位
    PIE1=_PIE1_TMR2IE_MASK;    
}
/**
 * 通过Timer2中断来调制占空比
 * 
 */
void PWMModulate(){
    //第一个周期占空比为25% ,k=257 ,CCPR1L=0x40, 
    //第二个周期占空比为50%，k=514
    //第三个周期占空比为75%，k=771
//    CCP1CONbits.CCP1X=0;
//    CCP1CONbits.CCP1Y=0;
    if(count==4){
        //当count==4的时候就是下一个的count=1,
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

//中断函数,PWM是一个周期进入一次中断
//51.4us进入一次中断

void __interrupt() PWM(void){
    //判断是否是timer2中断
    if(PIE1bits.TMR2IE&&PIR1bits.TMR2IF){
        //复位标志位
        PIR1bits.TMR2IF=0;
        count++;
        PWMModulate();
    }
}
void main(void) {
    
    PWMInit();
    //打开Timer2中断
    Timer2IEInit();
   
    while(1){
        
    };
    return;
}
