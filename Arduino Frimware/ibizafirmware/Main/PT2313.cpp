#include "PT2313.h"

/*
loudness:off
gain:0db
source:user assigned:
volume:muted or presetted
*/
void PT2313::initialize(byte src,bool muted){
	Wire.begin();
#if ARDUINO >= 157
	Wire.setClock(100000UL); // Set I2C frequency to 100kHz
#else
	TWBR = ((F_CPU / 100000UL) - 16) / 2; // Set I2C frequency to 100kHz
#endif
	if (muted){
		volume(63);//muted
	} else {
		volume(PT2313_DEFVOL);
	}
	audioSwitch_reg = 0x5C;
	source(src);
	balance(0);
    fader_balance(0,0);
	bass(0);
	treble(0);
}


void PT2313::source(byte val) { 
        val = val % 3; //range 0-2
	switch(val){
	case 0:
		bitClear(audioSwitch_reg,0);
		bitClear(audioSwitch_reg,1);
	break;
	case 1:
		bitSet(audioSwitch_reg,0);
		bitClear(audioSwitch_reg,1);
	break;
	case 2:
		bitClear(audioSwitch_reg,0);
		bitSet(audioSwitch_reg,1);
	break;
	}
	writeByte(audioSwitch_reg); 
}

void PT2313::gain(byte val) {//range 0-3
	switch(val){
	case 0://0db
		bitSet(audioSwitch_reg,3);
		bitSet(audioSwitch_reg,4);
	break;
	case 1://+3,75db
		bitClear(audioSwitch_reg,3);
		bitSet(audioSwitch_reg,4);
	break;
	case 2://+7.5db
		bitSet(audioSwitch_reg,3);
		bitClear(audioSwitch_reg,4);
	break;
	case 3://+11.25db
		bitClear(audioSwitch_reg,3);
		bitClear(audioSwitch_reg,4);
	break;
	}
	writeByte(audioSwitch_reg);
}

void PT2313::loudness(bool val) {
	if (val){
		bitClear(audioSwitch_reg,2);
	} else {
		bitSet(audioSwitch_reg,2);
	}
	writeByte(audioSwitch_reg);
}


void PT2313::volume(byte val) {
	val = boundary(val,0,0x3F);
	writeByte(PT2313_VOL_REG|(0x3F - val));
}

void PT2313::bass(int val){
	byte temp = eqsubroutine(val);
	writeByte(PT2313_BASS_REG|temp);
}

void PT2313::treble(int val){
	byte temp = eqsubroutine(val);
	writeByte(PT2313_TREBLE_REG|temp);
}

byte PT2313::eqsubroutine(int val){
	byte temp;
	val = boundary(val,-7,7);
	if (val < 0) {
		temp = 7 - abs(val);
	} else {
		temp = 15 - val;
	}
	return temp;
}
int PT2313::get_fader_val(){
    return fad;
}
int PT2313::get_balance_val(){
    return bal;
}

void PT2313::balance(int val) {
    bal=val;
    fad=get_fader_val();
    fader_balance(fad,bal);
}
void PT2313::fader(int val){
    bal=get_balance_val();
    fad=val;
    fader_balance(fad,bal);
    
}

void PT2313::fader_balance(int val2,int val){
    val2 = boundary(val2,-31,31);
    val= boundary(val,-31,31);
    if ((val2 == 0)&&(val==0)){
        writeByte(PT2313_L_ATT_REG|0x00);
        writeByte(PT2313_R_ATT_REG|0x00);
        writeByte(PT2313_L1_ATT_REG|0x00);
        writeByte(PT2313_R1_ATT_REG|0x00);
    }
    if((val2<0)&&(val==0)){
        writeByte(PT2313_L_ATT_REG|0x00);
        writeByte(PT2313_R1_ATT_REG|((byte)abs(val2)));
        writeByte(PT2313_R_ATT_REG|0x00);
        writeByte(PT2313_L1_ATT_REG|((byte)abs(val2)));
        
    }
    if((val2>0)&&(val==0)){
        writeByte(PT2313_L1_ATT_REG|0x00);
        writeByte(PT2313_R_ATT_REG|((byte)abs(val2)));
        writeByte(PT2313_R1_ATT_REG|0x00);
        writeByte(PT2313_L_ATT_REG|((byte)abs(val2)));
    }
     if((val2==0)&&(val<0)){
         writeByte(PT2313_L_ATT_REG|0x00);
         writeByte(PT2313_R_ATT_REG|((byte)abs(val)));
         writeByte(PT2313_L1_ATT_REG|0x00);
         writeByte(PT2313_R1_ATT_REG|((byte)abs(val)));
     }
    if((val2==0)&&(val>0)){
        writeByte(PT2313_L_ATT_REG|((byte)abs(val)));
        writeByte(PT2313_R_ATT_REG|0x00);
        writeByte(PT2313_L1_ATT_REG|((byte)abs(val)));
        writeByte(PT2313_R1_ATT_REG|0x00);
    }
    if((val2<0)&&(val<0)){
        writeByte(PT2313_L_ATT_REG|0x00);
        writeByte(PT2313_R_ATT_REG|((byte)abs(val)));
        writeByte(PT2313_R1_ATT_REG|((byte)abs(val2)));
        writeByte(PT2313_L1_ATT_REG|((byte)abs(val2)));
    }
    if((val2<0)&&(val>0)){
        writeByte(PT2313_L_ATT_REG|((byte)abs(val)));
        writeByte(PT2313_R_ATT_REG|0x00);
        writeByte(PT2313_R1_ATT_REG|((byte)abs(val2)));
        writeByte(PT2313_L1_ATT_REG|((byte)abs(val2)));
    }
    if((val2>0)&&(val<0)){
        writeByte(PT2313_L1_ATT_REG|0x00);
        writeByte(PT2313_R1_ATT_REG|((byte)abs(val2)));
        writeByte(PT2313_L_ATT_REG|((byte)abs(val)));
        writeByte(PT2313_R_ATT_REG|((byte)abs(val)));
        
    }
    if((val2>0)&&(val>0)){
        writeByte(PT2313_R1_ATT_REG|0x00);
        writeByte(PT2313_L1_ATT_REG|((byte)abs(val2)));
        writeByte(PT2313_L_ATT_REG|((byte)abs(val)));
        writeByte(PT2313_R_ATT_REG|((byte)abs(val)));
        
    }
}

int PT2313::boundary(int val,int min,int max){
	if (val < min) val = min;
	if (val > max) val = max;
	return val;
}
void PT2313::writeByte(byte val) {
	Wire.beginTransmission(PT2313_ADDR);
    Wire.write(val);
	Wire.endTransmission();
	delay(10);
}
