/*
 * gps.c
 *
 *  Created on: Feb 7, 2024
 *      Author: mahim
 */


#include "main.h"
#include <stdio.h>
#include <string.h>
#include "gps.h"

#define Min_To_Degree  0.01666666667
#define Sec_To_Degree  0.000277777778


extern UART_HandleTypeDef huart1;
extern char gps_info[100];
char gps_comp_info[100];
char str[100];
gpsdata gps;
gpsdata gps_rcv;
int dd,mm,yy;
int flag=0;
int GMT=530;
//strcpy(gps.Data,"hello");
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        // Process the received data
    	//printf("data is %s\n",gps.Data);
        gpsData(&gps);
    }
}
void gps_init(void)
{
//if(HAL_UARTEx_ReceiveToIdle(&huart1, (uint8_t*)gps.Data, 750, 500, 1000))
	HAL_UART_Receive_IT(&huart1,&gps.Data,500);
	//{
		printf("received start from MODULE\n");
	    printf("%s\n",gps.Data);
	    printf("received end  from MODULE\n");
	       //gpsData(&gps);
	    //return 0;
	/*}
	else
	{

		printf("Uart data not received\n");
		return 0;
	}*/
}


void gpsData(gpsdata *data)
{
	char *gga_ptr,*rmc_ptr;
	//gga_ptr=NULL;
	//rmc_ptr=NULL;
	int n=0;
	int rmc_len;
	printf("in func\n");
	//memset(data->Data,0,sizeof(data->Data));
	//HAL_UART_Receive(&huart3, data->Data, sizeof(data->Data),5000);
	//printf("string %s\n",data->Data);
	if(data->Data!=NULL)   //here changed
	{
	gga_ptr=strstr(data->Data,"GNGGA");
//	printf("gga %s\n",gga_ptr);
	rmc_ptr=strstr(data->Data,"GNRMC");
//	printf("rmc %s\n",rmc_ptr);
	}
	else
	{
		printf("no data in buffer\n");
	}

	//printf("string found\n");
	if(gga_ptr!=NULL)
	{
	if(*gga_ptr=='G')
	{
	   n=0;
	   printf("gga ptr copy start %s\n",gga_ptr);
	   strncpy(data->buffer,gga_ptr,80);
	   printf("gga ptr copy end %d\n",n);

	    printf("GGA string is %s\n", data->buffer);
     	//sprintf(data->buffer,"%s",data->buffer);
		sscanf(data->buffer,"GNGGA,%lf,%f,%c,%f,%c,%d,",&data->time,&data->latitude,&data->N_OR_S,&data->longitude,&data->E_OR_W,&data->fix);
	}
	else
		{
			printf("GGA string not found\n");
		}

	}
	else
	{
		printf("GGA string not found\n");
	}


	if(rmc_ptr!=NULL)
		{
		if(*rmc_ptr=='G')
				{
				   n=0;
				   printf("rmc ptr copy start %s\n",rmc_ptr);
				   strncpy(data->rmc_buffer,rmc_ptr,90);
				//   printf("rmc ptr copy end %s\n",rmc_ptr);
				   printf("RMC string is %s\n", data->rmc_buffer);
				   rmc_len=strlen(data->rmc_buffer);
				   if(rmc_len>=64)
				   {
				   					//sprintf(data->buffer,"%s",data->location);
				   					sscanf(data->rmc_buffer,"GNRMC,%*lf,%*c,%*f,%*c,%*f,%*c,%*f,%*f,%d,",&data->Date);
				   					//printf("date is %d\n",data->Date);
				   }
				}


	else
	{
		printf("RMC string not found\n");
	}
		}
	else
		{
			printf("RMC string not found\n");
		}


					//memset(data->Data,0,sizeof(data->Data));


		if(data->fix==1)
		{
			printf("valid fix\n");
			gpslocation_extraction(data);
		}
		else
		{
			sprintf(gps_info, "%s","invalid signal no fix, Unable to obtain accurate location.\n");
		}

		//memset(data->Data,0,sizeof(data->Data));
}


void gpslocation_extraction(gpsdata* data)
{
	double Deg_Val=0.0,Min_Val=0.0,Sec_Val=0.0,lon=0.0,lat=0.0;
	int hr,min,sec;
	lon=data->longitude;
	lat=data->latitude;
	if((data->E_OR_W=='E' || data->N_OR_S=='S')||(data->E_OR_W=='W' || data->N_OR_S=='N'))
	{
		Deg_Val=(int)(lon/100);
		Min_Val=(int)(lon-(Deg_Val*100));
		Sec_Val=((lon-(Deg_Val*100))-Min_Val)*100;
		data->longitude=(Deg_Val+(Min_Val*Min_To_Degree)+(Sec_Val*Sec_To_Degree));
		printf("longitude : %f\n",data->longitude);

		Deg_Val=(int)((lat/100));
		Min_Val=(int)(lat-(Deg_Val*100));
		Sec_Val=((lat-(Deg_Val*100))-Min_Val)*10;
		data->latitude=(Deg_Val+(Min_Val*Min_To_Degree)+(Sec_Val*Sec_To_Degree));
		printf("latitude : %f\n",data->latitude);
	}
	    /*hr=(int)(data->time)/10000;
	    min=(int)(data->time-(hr*10000))/100;
	    sec=(int)(data->time-((hr*10000)+(min*100)));*/


	    hr=(int)((data->time)/10000);
	   	min=(int)(data->time-(hr*10000))/100;
	   	sec=(int)(data->time-((hr*10000)+(min*100)));
	   	int ist_hr = hr + 5;  // 5 hours difference
	   	int ist_min = min + 30;  // 30 minutes difference
        if(ist_hr>=24)
        {
        	ist_hr-=24;
        }
	   	// Adjusting for overflow
	   	if (ist_min >= 60) {
	   	    ist_hr++;
	   	    ist_min -= 60;
	   	}
	    //sprintf(time_buf, "%02d:%02d:%02d",hr,min,sec);
	    dd=(data->Date)/10000;
	    mm=(data->Date-(dd*10000))/100;
	    yy=(data->Date-((dd*10000)+(mm*100)));
	    printf("time: %02d:%02d:%02d\n",ist_hr,ist_min,sec);
	    printf("date: %02d/%02d/%02d\n",dd,mm,yy);
		sprintf(gps_info,"latitude\:%f %c,\longitude\:%f %c,\ time\: \%02d:%02d:%02d,\date\: \%02d/%02d/%02d",data->latitude,data->N_OR_S, data->longitude,data->E_OR_W,ist_hr,ist_min,sec,dd,mm,yy);
		//sprintf(gps_info,"{\"latitude\":%f %c,\n\"longitude\":%f %c,\n \"time\": %02d:%02d:%02d, \n\"date\": %02d/%02d/%02d\n }",data->latitude,data->N_OR_S, data->longitude,data->E_OR_W,ist_hr,ist_min,sec,dd,mm,yy);

		//printf("json format is %s\n",json_str);
	   	//memset(data->Data,0,sizeof(data->Data));
		//memset(data->buffer,0,sizeof(data->buffer));
		//memset(data->rmc_buffer,0,sizeof(data->rmc_buffer));
}

