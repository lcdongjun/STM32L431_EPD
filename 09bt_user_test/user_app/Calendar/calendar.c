#include "calendar.h"
#include "malloc.h"
#include <string.h>
#include "rtc.h"
#include "DS3231.h"
#include "SystemInfo.h"

Calendar CalendarStruct;
		
// 获取每个月的天数，考虑闰年
int getDaysInMonth(int year, MonthName month) 
{
    switch (month) {
        case JANUARY: case MARCH: case MAY: case JULY:
        case AUGUST: case OCTOBER: case DECEMBER:
            return 31;
        case APRIL: case JUNE: case SEPTEMBER: case NOVEMBER:
            return 30;
        case FEBRUARY:
            // 判断是否为闰年
            return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
        default:
            return 30;
    }
}

// 计算某个日期是星期几（使用基姆拉尔森公式）
Weekday calculateWeekday(int year, int month, int day) {
    if (month == 1 || month == 2) {  // 把 1 月和 2 月视为前一年的 13 月和 14 月
        month += 12;
        year -= 1;
    }

    int Y = year;
    int m = month;
    int d = day;

    int w = (d + (13 * (m + 1)) / 5 + Y + (Y / 4) - (Y / 100) + (Y / 400)) % 7;

    return (Weekday)((w + 6) % 7);  // 让 0 对应到星期日
}


// 初始化某个月的信息
void initMonth(Month *month, u8 day)
{
    // 设置当月的天数
    month->days = getDaysInMonth(month->year, month->name);
    
    // 计算该月的第一天是星期几
    Weekday firstDayWeekday = calculateWeekday(month->year, month->name, 1);
    // 设置每一天的信息
    for (int i = 0; i < month->days; i++) {
        month->daysArray[i].day = i + 1;
        month->daysArray[i].weekday = (Weekday)((firstDayWeekday + i) % 7);  // 顺序增加
			
				if(month->daysArray[i].weekday == 0 ||month->daysArray[i].weekday == 6)
					month->daysArray[i].isHoliday = 1; //周六周天为节日
				else month->daysArray[i].isHoliday = 0; // 默认非节日
				
				if((i+1) == day&&month->isTomonth == 1){//判断是否为今天
					month->daysArray[i].isToday = 1;
				}
				else{
					month->daysArray[i].isToday = 0;
				}
    }
}

// 初始化一年的日历
void initYear(Calendar *CalendarStruct, u16 year, u8 month, u8 day) 
{
    CalendarStruct->Year.year = year;
    for (u8 i = 0; i < 12; i++) {
        CalendarStruct->Year.months[i].year = year;
       CalendarStruct->Year.months[i].name = (MonthName)(i + 1);
			
				if(i == month-1)
				{
					CalendarStruct->Year.months[i].isTomonth = 1;
				}
				else
				{
					CalendarStruct->Year.months[i].isTomonth = 0;
				}
				
        initMonth(&CalendarStruct->Year.months[i], day);
    }
}

// 显示单个日期
void drawDay(int x, int y, Day *day) 
{
	if(day->day<10)
	{
    Paint_Show_xNum(x+6, y, day->day, 24, 1, 1);
	}
	else
	{
		Paint_Show_xNum(x, y, day->day, 24, 1, 1);
	}
    // 标记节日
    if (day->isHoliday && !day->isToday) {
        Paint_Show_RoundRect(x-1 , y -4, x + 28, y + 22, 6, 1, 1, 0, 0); // 标记框
    }
		if (day->isToday){
				Paint_Show_RoundRect( x, y - 4, x + 28, y + 22, 6, 1, 1, 1, 1);
		}
}

// 显示整个月份
void drawMonth(Month *month, u16 x_start, u16 y_start) 
{
    // 绘制周标题
    u8 *daysOfWeek[] = {"8", "1", "2", "3", "4", "5", "6"};
    for (u8 i = 0; i < 7; i++) {
        Paint_Show_Str(x_start + i * 32, y_start, daysOfWeek[i], 24, 1, 1);
    }
    
    // 绘制日期
    int startX = x_start, startY = y_start + 32; // 开始绘制日期
		
    Weekday firstDayWeekday = month->daysArray[0].weekday;

    // 打印月初空格
    for (int i = 0; i < firstDayWeekday; i++) {
        startX += 32;
    }

    // 打印日期
    for (u8 i = 0; i < month->days; i++) {

        drawDay(startX, startY, &month->daysArray[i]);
        
        // 每七天换行
        if ((firstDayWeekday + i + 1) % 7 == 0) {
            startY += 32;
            startX = x_start;
        } else {
            startX += 32;
        }
    }
}


void displayCalendar(u16 x_start, u16 y_start)
{
		uint8_t *Partial_Calendar_Image;
		Partial_Calendar_Image = mymalloc(SRAMIN,(32*7 / 8 + 6) * (32*7+6));
		if (Partial_Calendar_Image == NULL)
    {
        printf("Failed to apply for partial memory...\r\n");
    }
		Paint_NewImage(Partial_Calendar_Image, 32*7 + 6, 32*7+6, 0, EPD_WHITE);
		Paint_Clear(EPD_WHITE);
		
		for(u8 i=0;i<12;i++)
		{
			if(CalendarStruct.Year.months[i].isTomonth ==1 )
			{
				drawMonth(&CalendarStruct.Year.months[i], 6, 6);
				break;
			}
		}
		EPD_4IN2_V2_PartialDisplay(Partial_Calendar_Image, x_start, y_start, x_start+(32*7 + 6), y_start+(32*7+6));
		myfree(SRAMIN,Partial_Calendar_Image);
		
}
void displayTime(u16 x_start, u16 y_start)
{
		uint8_t *Partial_Time_Image;
		Partial_Time_Image = mymalloc(SRAMIN,(120 / 8 ) * (48+4));
		if (Partial_Time_Image == NULL)
    {
        printf("Failed to apply for partial memory...\r\n");
    }
		Paint_NewImage(Partial_Time_Image, 120, 52, 0, EPD_WHITE);
		Paint_Clear(EPD_WHITE);
		Paint_Show_Char(1+(48*1),2,':',48,1,1);
		if(CalendarStruct.Time.Hour<10)
		{
			Paint_Show_xNum(1,5,0,48,1,0);
			Paint_Show_xNum(1+24,5,CalendarStruct.Time.Hour,48,1,0);
		}
		else
		{
			Paint_Show_xNum(1,5,CalendarStruct.Time.Hour,48,1,0);
		}
		
		if(CalendarStruct.Time.Min<10)
		{
			Paint_Show_xNum(1+(48*1)+24,5,0,48,1,0);
			Paint_Show_xNum(1+(48*2),5,CalendarStruct.Time.Min,48,1,0);
		}
		else
		{
			Paint_Show_xNum(1+(48*1)+24,5,CalendarStruct.Time.Min,48,1,0);
		}
		
		EPD_4IN2_V2_PartialDisplay(Partial_Time_Image, x_start, y_start, x_start+120, y_start+52 );
		myfree(SRAMIN,Partial_Time_Image);
		
}

// 主程序，初始化并显示日历
u8 new_date,old_date,new_min,old_min = 99;

void initCalendar(u8 Calendar_x, u8 Calendar_y, u8 Time_x, u8 Time_y)
{
		static uint8_t flage = 0;
		CalendarStruct.Time.Year = SystemInfo.DT.year+2000;
		CalendarStruct.Time.Month = SystemInfo.DT.month;
		CalendarStruct.Time.Day = SystemInfo.DT.dayofmonth;
		CalendarStruct.Time.Hour = SystemInfo.DT.hour;
		CalendarStruct.Time.Min = SystemInfo.DT.minute;
		CalendarStruct.Time.Sec = SystemInfo.DT.second;
	
		new_date = SystemInfo.DT.dayofmonth;
		new_min = SystemInfo.DT.minute;
		if(new_min!=old_min)
		{
			old_min = new_min;
			flage++;
			PrintDateTime(&SystemInfo.DT);
			displayTime(Time_x,Time_y);

			if(flage >= 10)
			{
				printf("Repetition\r\n");
				displayTime(Time_x,Time_y);
				vTaskDelay(pdMS_TO_TICKS(1500));
				displayTime(Time_x,Time_y);
				flage =0;
			}
		}
		if(new_date!= old_date)
		{
			old_date = new_date;
			initYear(&CalendarStruct,SystemInfo.DT.year+2000,SystemInfo.DT.month,SystemInfo.DT.dayofmonth);
			displayCalendar(Calendar_x,Calendar_y);
		}

}
