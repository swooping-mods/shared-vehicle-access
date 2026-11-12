#ifndef MOON_BORROW_FRAME_ADMISSION_H
#define MOON_BORROW_FRAME_ADMISSION_H
/* Peer callbacks must neither inspect peer equipment nor consume local edges.
 * Unknown local identity consumes a blocked sample to prevent a held-key edge
 * when identity becomes readable again. */
static int borrow_frame_admit(PTR walker,PTR *world){
 PTR local=0;*world=0;
 if(!moon_local_actor_read(&local)||!local){
  moon_input_action("shared-vehicle-access.borrow","Alt+F",0);return 0;
 }
 if(local!=walker)return 0;
 int ok=1;PTR mounted=0;DWORD pid=0;
 field_value(walker,world_f,world);field_value(walker,mounted_f,&mounted);
 window_pid(foreground(),&pid);
 int ready=alive(*world)&&!alive(mounted)&&pid==GetCurrentProcessId()&&
  vehicle_read_boolean(focus_m,0,&ok)&&!vehicle_read_boolean(typing_m,0,&ok)&&
  !vehicle_read_boolean(blocked_m,walker,&ok)&&!vehicle_read_boolean(menu_m,walker,&ok)&&
  !vehicle_read_boolean(restoring_m,*world,&ok)&&ok;
 int edge=moon_input_action("shared-vehicle-access.borrow","Alt+F",ready).pressed;
 return ready&&edge;
}
#endif
