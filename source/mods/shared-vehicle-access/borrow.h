#ifndef MOON_SHARED_VEHICLE_BORROW_H
#define MOON_SHARED_VEHICLE_BORROW_H
/* Keep every transaction input alive, including strings replaced by handover.
 * Give may commit before Take fails: report that state without replay/rollback. */
static void borrow_vehicle(PTR world,PTR equipment,PTR vehicle,PTR donor){
 int ok=1;DRoot roots[8]={0};PTR inputs[]={world,equipment,vehicle,donor};
 for(int i=0;i<4;i++){roots[i]=dhold(inputs[i]);if(!roots[i])goto done;}
 int index=vehicle_read_integer(index_m,vehicle,&ok);
 roots[4]=dhold(mf_vehicle_storage(vehicle,&ok));PTR storage=dget(roots[4]);
 roots[5]=dhold(call(mf.local,0,0,&ok));PTR player=dget(roots[5]);
 PTR owned=mf_lookup(world,storage,&ok);MoonFleetRecord record={0};
 if(!ok||!storage||!player||!mf_read(owned,&record)||!record.deployed||
    !record.owner||!moon_fleet_unoccupied(&mf_queries,&record))goto done;
 roots[6]=dhold(record.owner);roots[7]=dhold(da.new_object(handover_class));
 if(!roots[6]||!roots[7])goto done;
 PTR box=dget(roots[7]),data=box?unbox(box):0;
 if(!data){event("borrow-handover-buffer-unavailable");goto done;}
 PTR message=0;void *give[]={donor,&index,storage,player,data,&message};
 PTR granted=call(give_m,world,give,&ok);
 PTR decision=ok&&granted?unbox(granted):0;
 if(!decision){event("borrow-handover-uncertain");goto done;}
 if(!*(unsigned char*)decision){event("borrow-handover-refused");goto done;}
 box=dget(roots[7]);data=box?unbox(box):0;
 if(!data){event("borrowed-vehicle-handover-entry-pending");goto done;}
 void *take[]={equipment,data};PTR taken=call(take_m,world,take,&ok);
 if(!ok||!alive(taken)){event("borrowed-vehicle-handover-entry-pending");goto done;}
 /* A returned vehicle must still be this exact instance and retain its buyer.
  * A failed readback is observable, never permission for a guessed reversal. */
 PTR returned=mf_vehicle_storage(taken,&ok);owned=mf_lookup(world,storage,&ok);
 if(!ok||!mf_equal(returned,storage)||!mf_read(owned,&record)||
    !mf_equal(record.owner,dget(roots[6]))){event("borrow-handover-readback-failed");goto done;}
 if(vehicle_read_boolean(enter_m,taken,&ok)&&ok)event("borrowed-vehicle-entered");
 else event("borrowed-vehicle-handover-entry-pending");
done:
 for(int i=0;i<8;i++)ddrop(&roots[i]);
}
#endif
