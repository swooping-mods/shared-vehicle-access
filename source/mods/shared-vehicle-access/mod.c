/* Borrowing uses the existing handover transaction and never changes the buyer. */
#include "base.h"
#include "../../runtime/native/input.h"
#include "../vehicle-framework/sdk/fleet_adapter.h"
static PTR handover_class;
static PTR aim_f,crew_aim_f,equipment_owner_f,index_m,give_m,take_m,enter_m,driven_m,ask_m;
static int before(int id,Context *c){(void)id;if(!on_thread())return 0;int edge=moon_input_action("shared-vehicle-access.borrow","Alt+F",1).pressed,ok=1;PTR walker=(PTR)c->rcx,world=0,mounted=0;field_value(walker,world_f,&world);field_value(walker,mounted_f,&mounted);DWORD pid=0;window_pid(foreground(),&pid);
 if(!edge||!alive(world)||alive(mounted)||pid!=GetCurrentProcessId()||!boolean(focus_m,0,&ok)||boolean(typing_m,0,&ok)||boolean(blocked_m,walker,&ok)||boolean(menu_m,walker,&ok)||boolean(restoring_m,world,&ok)||!ok)return 0;
 PTR equipment=component(walker,"MoonPrototypeEquipment",&ok),v=0,remote=0;if(!equipment||!ok)return 0;field_value(equipment,aim_f,&v);field_value(equipment,crew_aim_f,&remote);
 if(alive(remote)){void *a[]={remote};call(ask_m,equipment,a,&ok);event(ok?"borrow-request-sent":"borrow-request-failed");return 0;}
 if(!alive(v)||boolean(driven_m,v,&ok)||!ok)return 0;PTR donor=0;field_value(v,equipment_owner_f,&donor);if(!donor)return 0;
 if(donor==equipment){boolean(enter_m,v,&ok);return 0;}int index=integer(index_m,v,&ok);PTR storage=mf_vehicle_storage(v,&ok),player=call(mf.local,0,0,&ok),message=0;if(!ok||!storage||!player)return 0;DRoot handover=dhold(da.new_object(handover_class));if(!handover)return 0;
 /* GiveVehicle preserves the MoonOwnedVehicle.owner purchase record and moves
  * only the physical holder. Its native checks still reject occupied vehicles. */
 void *give[]={donor,&index,storage,player,unbox(dget(handover)),&message};PTR granted=call(give_m,world,give,&ok);if(!ok||!granted||!*(unsigned char*)unbox(granted)){event("borrow-handover-refused");ddrop(&handover);return 0;}
 void *take[]={equipment,unbox(dget(handover))};PTR vehicle=call(take_m,world,take,&ok);ddrop(&handover);if(ok&&alive(vehicle)&&boolean(enter_m,vehicle,&ok)&&ok)event("borrowed-vehicle-entered");else event("borrowed-vehicle-handover-entry-pending");return 0;
}
int MoonLab_ModStart(PTR domain,PTR game){if(!common(domain,game,"shared-vehicle-access.jsonl")||!mf_bind(game,0))return 0;
 (void)pressed;event("starting");
#define M(F,T,N,S) F=dmethod("MoonCleaners",T,N,S);if(!F){event("method-missing:" T "::" N);return 0;}
 M(index_m,"MoonPrototypeVehicle","get_Index","");M(give_m,"MoonWorld","GiveVehicle","MoonCleaners.MoonPrototypeEquipment;System.Int32;System.String;System.String;MoonCleaners.MoonWorld.VehicleHandover&;System.String&");M(take_m,"MoonWorld","TakeVehicle","MoonCleaners.MoonPrototypeEquipment;MoonCleaners.MoonWorld.VehicleHandover");M(enter_m,"MoonPrototypeVehicle","Enter","");M(driven_m,"MoonPrototypeVehicle","get_IsDriven","");M(ask_m,"MoonPrototypeEquipment","AskForCrewVehicle","MoonCleaners.MoonRemoteVehicle");
#undef M
 /* Let the runtime own the value buffer and trace its managed fields. */
 PTR (*from_type)(PTR)=(PTR(*)(PTR))GetProcAddress(game,"il2cpp_class_from_type");
 int (*value_size)(PTR,DWORD*)=(int(*)(PTR,DWORD*))GetProcAddress(game,"il2cpp_class_value_size");
 unsigned char (*is_value)(PTR)=(unsigned char(*)(PTR))GetProcAddress(game,"il2cpp_class_is_valuetype");
 if(!from_type||!value_size||!is_value){event("value-api-missing");return 0;}handover_class=from_type(da.param(take_m,1));if(!handover_class||!is_value(handover_class)){event("handover-class-invalid");return 0;}DWORD alignment=0;
 int size=value_size(handover_class,&alignment);if(size<=0||size>4096||alignment>16){event("handover-layout-invalid");return 0;}
 PTR k=find_class("MoonCleaners","MoonPrototypeEquipment");aim_f=get_field(k,"aimedVehicle");crew_aim_f=get_field(k,"aimedCrewVehicle");equipment_owner_f=get_field(find_class("MoonCleaners","MoonPrototypeVehicle"),"owner");if(!aim_f||!crew_aim_f||!equipment_owner_f){event("vehicle-field-missing");return 0;}if(!update_hook(game)){event("update-hook-shape-failed");return 0;}before_hook=before;if(!install_hooks()){event("hook-install-failed");return 0;}event("ready");return 1;}
