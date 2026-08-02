#ifndef _SERVO_H
#define _SERVO_H

void Servo_Init(void);
void Servo_move_by_degress(float rotation, float pitch);
void Servo_move_increment(float x, float y);
void Servo_move_to_traget_close_loop(float target_x, float target_y);

#endif
