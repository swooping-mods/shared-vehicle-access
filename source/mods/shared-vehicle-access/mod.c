/* Borrowing uses the existing handover transaction and never changes the buyer. */
#include "base.h"
#include "../../runtime/native/input.h"
#include "../../runtime/native/local_actor.h"
#include "../vehicle-framework/sdk/fleet_adapter.h"
static PTR handover_class;
static PTR aim_f,crew_aim_f,equipment_owner_f,index_m,give_m,take_m,enter_m,driven_m,ask_m;
#include "borrow.h"
#include "frame_admission.h"
static int before(int id,Context *c){(void)id;if(!on_thread())return 0;int ok=1;PTR walker=(PTR)c->rcx,world=0;
 if(!borrow_frame_admit(walker,&world))return 0;
 PTR equipment=component(walker,"MoonPrototypeEquipment",&ok),v=0,remote=0;if(!equipment||!ok)return 0;field_value(equipment,aim_f,&v);field_value(equipment,crew_aim_f,&remote);
 if(alive(remote)){void *a[]={remote};call(ask_m,equipment,a,&ok);event(ok?"borrow-request-sent":"borrow-request-failed");return 0;}
 if(!alive(v)||vehicle_read_boolean(driven_m,v,&ok)||!ok)return 0;PTR donor=0;field_value(v,equipment_owner_f,&donor);if(!alive(donor))return 0;
 if(donor==equipment){vehicle_read_boolean(enter_m,v,&ok);return 0;}
 borrow_vehicle(world,equipment,v,donor);return 0;
}
int MoonLab_ModStart(PTR domain,PTR game){if(!common(domain,game,"shared-vehicle-access.jsonl"))return 0;
 if(!moon_local_actor_bind()){event("local-actor-binding-failed");return 0;}
 if(!mf_bind(game,1)){event("fleet-passenger-binding-failed");return 0;}
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
