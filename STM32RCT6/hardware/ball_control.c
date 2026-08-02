#include "ball_control.h"

//	01		02		03
//
//		11	12	13
//
//	04	14	05	15	06
//
//		16	17	18
//
//	07		08		09

#define NUM_POINTS 19	   			//总共17个点，便于记忆，将0和10去掉，区域一就是对应points[1]

Point points[NUM_POINTS];			//1-9对应的是区域，中间八个是无区域

uint8_t times = 0;					//计时次数，一次等于一秒，每一次任务用完一定要清零

void ball_control_Init(void)
{
	Timer4_Init();
	/*题目要求的九个区域*/
	points[1].x = 185;			//实际177，修正之后185
    points[1].y = 70;           //实际97，修正之后70

	points[2].x = 322;			//实际320，修正之后322
    points[2].y = 58;			//实际97，修正之后58

	points[3].x = 464;			//实际465，修正之后464
    points[3].y = 70;			//实际97，修正之后70
	
	points[4].x = 177;			//实际177，修正之后177
    points[4].y = 255;			//实际240，修正之后255

	points[5].x = 323;			//实际320，修正之后323
    points[5].y = 238;			//实际240，修正之后238

	points[6].x = 475;			//实际465，修正之后475
    points[6].y = 240;			//实际240，修正之后240

	points[7].x = 185;			//实际177，修正之后185
    points[7].y = 440;			//实际383，修正之后440

	points[8].x = 323;			//实际320，修正之后323
    points[8].y = 423;			//实际383，修正之后423
	
	points[9].x = 510;			//实际465，修正之后510
    points[9].y = 450;			//实际383，修正之后450
	
	/*自定的八个区域*/
	points[11].x = (points[1].x + points[5].x) / 2;
    points[11].y = (points[1].y + points[5].y) / 2;
	
	points[12].x = ((points[1].x + points[5].x) / 2 + (points[3].x + points[5].x) / 2) / 2;
    points[12].y = (points[2].y + points[5].y) / 2;
	
	points[13].x = (points[3].x + points[5].x) / 2;
    points[13].y = (points[3].y + points[5].y) / 2;
	
	points[14].x = (points[4].x + points[5].x) / 2;
    points[14].y = ((points[1].y + points[5].y) / 2 + (points[7].y + points[5].y) / 2) / 2;
	
	points[15].x = (points[6].x + points[5].x) / 2;
    points[15].y = ((points[3].y + points[5].y) / 2 + (points[9].y + points[5].y) / 2) / 2;
	
	points[16].x = (points[7].x + points[5].x) / 2;
    points[16].y = (points[7].y + points[5].y) / 2;
	
	points[17].x = ((points[7].x + points[5].x) / 2 + (points[9].x + points[5].x) / 2) / 2;
    points[17].y = (points[8].y + points[5].y) / 2;
	
	points[18].x = (points[9].x + points[5].x) / 2;
    points[18].y = (points[9].y + points[5].y) / 2;
}

//进入目标点，多长时间后跳出
void control_target(float target_x, float target_y, uint8_t dt)
{
	while(1)																//进入while循环
	{
		Servo_move_to_traget_close_loop(target_x, target_y);				//进入目标点
			
		if(times == dt)														//计时dt
		{
			times = 0;														//计时完成，定时器计时清零
			break;															//跳出循环
		}
	}	
}

//由一个点走到下一个点，加入了判断是否会经过区域五，如果是，会选择绕过区域五
void target1_to_target2(uint8_t m, uint8_t n)
{
	control_target(points[m].x, points[m].y, 3);
	
	if(m + n == 10)
	{
		if(m == 1)
		{
			control_target(points[11].x, points[11].y, 1);
			control_target(points[12].x, points[12].y, 1);
			control_target(points[13].x, points[13].y, 1);
			control_target(points[15].x, points[15].y, 1);
			control_target(points[18].x, points[18].y, 1);
		}
		else if(m == 2)
		{
			control_target(points[13].x, points[13].y, 1);
			control_target(points[15].x, points[15].y, 1);
			control_target(points[18].x, points[18].y, 1);
		}
		else if(m == 3)
		{
			control_target(points[13].x, points[13].y, 1);
			control_target(points[15].x, points[15].y, 1);
			control_target(points[18].x, points[18].y, 1);
			control_target(points[17].x, points[17].y, 1);
			control_target(points[16].x, points[16].y, 1);
		}
		else if(m == 4)
		{
			control_target(points[11].x, points[11].y, 1);
			control_target(points[12].x, points[12].y, 1);
			control_target(points[13].x, points[13].y, 1);
		}			
		else if(m == 6)
		{
			control_target(points[18].x, points[18].y, 1);	
			control_target(points[17].x, points[17].y, 1);	
			control_target(points[16].x, points[16].y, 1);	
		}			
		else if(m == 7)
		{	
			control_target(points[16].x, points[16].y, 1);
			control_target(points[14].x, points[14].y, 1);
			control_target(points[11].x, points[11].y, 1);
			control_target(points[12].x, points[12].y, 1);
			control_target(points[13].x, points[13].y, 1);
		}			
		else if(m == 8)
		{
			control_target(points[16].x, points[16].y, 1);
			control_target(points[14].x, points[14].y, 1);
			control_target(points[11].x, points[11].y, 1);
		}			
		else if(m == 9)
		{
			control_target(points[18].x, points[18].y, 1);
			control_target(points[17].x, points[17].y, 1);
			control_target(points[16].x, points[16].y, 1);
			control_target(points[14].x, points[14].y, 1);
			control_target(points[11].x, points[11].y, 1);
		}
	}
	
	control_target(points[n].x, points[n].y, 3);
			
}

void ball_control(void)
{
	//将小球放置在区域2，控制小球在区域内停留不少于5秒
	while(mode == 1)
	{
		//USART1_printf("%f,%f\n",points_x, points_y);
		Servo_move_to_traget_close_loop(points[2].x, points[2].y);			//进入区域2
	}
	
	//在15秒内，控制小球从区域1进入区域5，在区域5停留不少于2秒
	//细分为先进入区域11，在进入区域5
	while(mode == 2)
	{
		times = 0;															//定时器计时清零
		TIM_Cmd(TIM4, ENABLE);												//定时器打开
		
		control_target(points[1].x, points[1].y, 5);
		
		control_target(points[11].x, points[11].y, 3);
		
		TIM_Cmd(TIM4, DISABLE);												//定时器关闭
		
		while(mode == 2)
		{
			Servo_move_to_traget_close_loop(points[5].x, points[5].y);		//进入区域5
		}
	}
	
	//控制小球从区域1进入区域4，在区域4停留不少于2秒
	//然后再进入区域5小球在区域5停留不少于2秒
	//完成以上两个动作总时间不超过20
	while(mode == 3)
	{
		times = 0;															//定时器计时清零
		TIM_Cmd(TIM4, ENABLE);												//定时器打开
		
		control_target(points[1].x, points[1].y, 3);
		
		control_target(points[4].x, points[4].y, 8);
		
		control_target(points[14].x, points[14].y, 2);
		
		TIM_Cmd(TIM4, DISABLE);												//定时器关闭
		
		while(mode == 3)
		{
			Servo_move_to_traget_close_loop(points[5].x, points[5].y);		//进入区域5
		}	
	}
	
	//在30秒内控制小球从区域1进入区域9且在区域9停留不少于2秒
	//1->11->12->13->15->18->9
	while(mode == 4)
	{
		times = 0;															//定时器计时清零
		TIM_Cmd(TIM4, ENABLE);												//定时器打开
		
		control_target(points[1].x, points[1].y, 5);
		
		control_target(points[11].x, points[11].y, 2);
		
		control_target(points[12].x, points[12].y, 2);
		
		control_target(points[13].x, points[13].y, 2);
		
		control_target(points[15].x, points[15].y, 3);
		
		control_target(points[18].x, points[18].y, 2);
		
		TIM_Cmd(TIM4, DISABLE);												//定时器关闭
		
		while(mode == 4)
		{
			Servo_move_to_traget_close_loop(points[9].x, points[9].y);		//进入区域9
		}	
	}
	
	//在40秒内，控制小球从区域1出发先后进入区域 2、区域 6，停止于区域9在区域9中停留时间不少于2秒
	//1->2->13->6->9
	while(mode == 5)
	{
		times = 0;															//定时器计时清零
		TIM_Cmd(TIM4, ENABLE);												//定时器打开
		
		control_target(points[1].x, points[1].y, 5);
		
		control_target(points[2].x, points[2].y, 2);
		
		control_target(points[13].x, points[13].y, 2);
		
		control_target(points[6].x, points[6].y, 3);
		
		TIM_Cmd(TIM4, DISABLE);												//定时器关闭
		
		while(mode == 5)
		{
			Servo_move_to_traget_close_loop(points[9].x, points[9].y);		//进入区域9
		}	
	}
	
	//在40秒内，控制小球从区域1出发先后进入区域 2、区域 6，停止于区域9在区域9中停留时间不少于2秒
	//1->2->13->6->9
	while(mode == 5)
	{
		times = 0;															//定时器计时清零
		TIM_Cmd(TIM4, ENABLE);												//定时器打开
		
		control_target(points[1].x, points[1].y, 5);
		
		control_target(points[2].x, points[2].y, 2);
		
		control_target(points[13].x, points[13].y, 2);
		
		control_target(points[6].x, points[6].y, 3);
		
		TIM_Cmd(TIM4, DISABLE);												//定时器关闭
		
		while(mode == 5)
		{
			Servo_move_to_traget_close_loop(points[9].x, points[9].y);		//进入区域9
		}	
	}
	
	//在40秒内，控制小球从区域A出发、先后进入区域B、区域C停止于区域D 
	//测试现场用键盘依次设置区域编号A、B、C、D控制小球完成动作
	//A->B->C->D
	while(mode == 6)
	{
		times = 0;															//定时器计时清零
		TIM_Cmd(TIM4, ENABLE);												//定时器打开
		
		control_target(points[A].x, points[A].y, 3);
		
		target1_to_target2(A, B);
		target1_to_target2(B, C);
		target1_to_target2(C, D);
		
		TIM_Cmd(TIM4, DISABLE);												//定时器关闭
		
		while(mode == 6)
		{
			Servo_move_to_traget_close_loop(points[D].x, points[D].y);		//进入区域D
		}	
	}
	
	//小球从区域4出发作环绕区域5的运动（不进入）运动不少于3周后停止于区域9，且保持不少于2秒
	//4->11->12->13->15->18->17->16->14----->9
	while(mode == 7)
	{
		times = 0;															//定时器计时清零
		TIM_Cmd(TIM4, ENABLE);												//定时器打开
		
		control_target(points[4].x, points[4].y, 5);
		
		control_target(points[11].x, points[11].y, 1);
		control_target(points[12].x, points[12].y, 1);
		control_target(points[13].x, points[13].y, 1);
		control_target(points[15].x, points[15].y, 1);
		control_target(points[18].x, points[18].y, 1);
		control_target(points[17].x, points[17].y, 1);
		control_target(points[16].x, points[16].y, 1);
		control_target(points[14].x, points[14].y, 1);
	
		control_target(points[11].x, points[11].y, 1);	
		control_target(points[12].x, points[12].y, 1);
		control_target(points[13].x, points[13].y, 1);
		control_target(points[15].x, points[15].y, 1);
		control_target(points[18].x, points[18].y, 1);
		control_target(points[17].x, points[17].y, 1);
		control_target(points[16].x, points[16].y, 1);
		control_target(points[14].x, points[14].y, 1);
		
		control_target(points[11].x, points[11].y, 1);
		control_target(points[12].x, points[12].y, 1);
		control_target(points[13].x, points[13].y, 1);
		control_target(points[15].x, points[15].y, 1);
		control_target(points[18].x, points[18].y, 1);
		control_target(points[17].x, points[17].y, 1);
		control_target(points[16].x, points[16].y, 1);
		control_target(points[14].x, points[14].y, 1);
		
		control_target(points[11].x, points[11].y, 1);
		control_target(points[12].x, points[12].y, 1);
		control_target(points[13].x, points[13].y, 1);
		control_target(points[15].x, points[15].y, 1);
		control_target(points[18].x, points[18].y, 2);
		
		TIM_Cmd(TIM4, DISABLE);												//定时器关闭
		
		while(mode == 7)
		{
			Servo_move_to_traget_close_loop(points[9].x, points[9].y);		//进入区域9
		}	
	}
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		times++;
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

