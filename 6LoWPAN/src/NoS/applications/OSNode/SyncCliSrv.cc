#include "AppTasks.h"




sc_core::sc_event cli1; // systemc channel
sc_core::sc_event cli2; // systemc channel
sc_core::sc_event cli3; // systemc channel
sc_core::sc_event cli4; // systemc channel
sc_core::sc_event cli5; // systemc channel
sc_core::sc_event cli6; // systemc channel
sc_core::sc_event cli7; // systemc channel
sc_core::sc_event cli8; // systemc channel
sc_core::sc_event cli9; // systemc channel
sc_core::sc_event cli10; // systemc channel
sc_core::sc_event cli11; // systemc channel
sc_core::sc_event cli12; // systemc channel
sc_core::sc_event cli13; // systemc channel
sc_core::sc_event cli14; // systemc channel
sc_core::sc_event cli15; // systemc channel

bool flag1=0;
bool flag2=0;
bool flag3=0;
bool flag4=0;
bool flag5=0;
bool flag6=0;
bool flag7=0;
bool flag8=0;
bool flag9=0;
bool flag10=0;
bool flag11=0;
bool flag12=0;
bool flag13=0;
bool flag14=0;
bool flag15=0;

unsigned long cli_commu[CLI_NUM_MAX] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int schd[CLI_NUM_MAX] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};


void cli_wait(int NodeID){//wait for server to be available

  int cliID=NodeID;
  if(cliID==1&&flag1==0) { sc_core::wait(cli1);}
  if(cliID==2&&flag2==0) { sc_core::wait(cli2);}
  if(cliID==3&&flag3==0) { sc_core::wait(cli3);}
  if(cliID==4&&flag4==0) { sc_core::wait(cli4);}
  if(cliID==5&&flag5==0) { sc_core::wait(cli5);}
  if(cliID==6&&flag6==0) { sc_core::wait(cli6);}
  if(cliID==7&&flag7==0) { sc_core::wait(cli7);}
  if(cliID==8&&flag8==0) { sc_core::wait(cli8);}
  if(cliID==9&&flag9==0) { sc_core::wait(cli9);}
  if(cliID==10&&flag10==0) {sc_core::wait(cli10);}
  if(cliID==11&&flag11==0) {sc_core::wait(cli11);}
  if(cliID==12&&flag12==0) {sc_core::wait(cli12);}
  if(cliID==13&&flag13==0) {sc_core::wait(cli13);}
  if(cliID==14&&flag14==0) {sc_core::wait(cli14);}
  if(cliID==15&&flag15==0) {sc_core::wait(cli15);}


  if(cliID==1)  flag1=0;
  if(cliID==2)  flag2=0;
  if(cliID==3)  flag3=0;
  if(cliID==4)  flag4=0;
  if(cliID==5)  flag5=0;
  if(cliID==6)  flag6=0;
  if(cliID==7)  flag7=0;
  if(cliID==8)  flag8=0;
  if(cliID==9)  flag9=0;
  if(cliID==10) flag10=0;
  if(cliID==11) flag11=0;
  if(cliID==12) flag12=0;
  if(cliID==13) flag13=0;
  if(cliID==14) flag14=0;
  if(cliID==15) flag15=0;


}

void srv_wait(int NodeID){//wait for server to be available


}

void srv_send(int NodeID, int cliID) //send availibility info from server side
{

  if(cliID==1){cli1.notify(sc_core::SC_ZERO_TIME);flag1=1;}
  if(cliID==2){cli2.notify(sc_core::SC_ZERO_TIME);flag2=1;}
  if(cliID==3){cli3.notify(sc_core::SC_ZERO_TIME);flag3=1;}
  if(cliID==4){cli4.notify(sc_core::SC_ZERO_TIME);flag4=1;}
  if(cliID==5){cli5.notify(sc_core::SC_ZERO_TIME);flag5=1;}
  if(cliID==6){cli6.notify(sc_core::SC_ZERO_TIME);flag6=1;}
  if(cliID==7){cli7.notify(sc_core::SC_ZERO_TIME);flag7=1;}
  if(cliID==8){cli8.notify(sc_core::SC_ZERO_TIME);flag8=1;}
  if(cliID==9){cli9.notify(sc_core::SC_ZERO_TIME);flag9=1;}
  if(cliID==10){cli10.notify(sc_core::SC_ZERO_TIME);flag10=1;}
  if(cliID==11){cli11.notify(sc_core::SC_ZERO_TIME);flag11=1;}
  if(cliID==12){cli12.notify(sc_core::SC_ZERO_TIME);flag12=1;}
  if(cliID==13){cli13.notify(sc_core::SC_ZERO_TIME);flag13=1;}
  if(cliID==14){cli14.notify(sc_core::SC_ZERO_TIME);flag14=1;}
  if(cliID==15){cli15.notify(sc_core::SC_ZERO_TIME);flag15=1;}

}





void cli_send(int NodeID, int cliID) //send availibility info from server side
{


}





void srv_send_dat(int NodeID,  int from_cliID, int cliID,  int image_count)
{

  if(cliID==1&&flag1==0) { sc_core::wait(cli1);}
  if(cliID==2&&flag2==0) { sc_core::wait(cli2);}
  if(cliID==3&&flag3==0) { sc_core::wait(cli3);}
  if(cliID==4&&flag4==0) { sc_core::wait(cli4);}
  if(cliID==5&&flag5==0) { sc_core::wait(cli5);}
  if(cliID==6&&flag6==0) { sc_core::wait(cli6);}
  if(cliID==7&&flag7==0) { sc_core::wait(cli7);}
  if(cliID==8&&flag8==0) { sc_core::wait(cli8);}
  if(cliID==9&&flag9==0) { sc_core::wait(cli9);}
  if(cliID==10&&flag10==0) {sc_core::wait(cli10);}
  if(cliID==11&&flag11==0) {sc_core::wait(cli11);}
  if(cliID==12&&flag12==0) {sc_core::wait(cli12);}
  if(cliID==13&&flag13==0) {sc_core::wait(cli13);}
  if(cliID==14&&flag14==0) {sc_core::wait(cli14);}
  if(cliID==15&&flag15==0) {sc_core::wait(cli15);}


  if(cliID==1)  flag1=0;
  if(cliID==2)  flag2=0;
  if(cliID==3)  flag3=0;
  if(cliID==4)  flag4=0;
  if(cliID==5)  flag5=0;
  if(cliID==6)  flag6=0;
  if(cliID==7)  flag7=0;
  if(cliID==8)  flag8=0;
  if(cliID==9)  flag9=0;
  if(cliID==10) flag10=0;
  if(cliID==11) flag11=0;
  if(cliID==12) flag12=0;
  if(cliID==13) flag13=0;
  if(cliID==14) flag14=0;
  if(cliID==15) flag15=0;


}

