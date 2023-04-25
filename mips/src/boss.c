#include <stdio.h>
#include "shell.h"

void offset_val_with_sign(int32_t BASE, int32_t OFFSET_VALUE, int32_t *output , uint8_t check_over_flow)
{
    int32_t new_output= 0x0;

    if (OFFSET_VALUE>>15 ==1)
    {
        new_output = (BASE-((0X7FFF- (OFFSET_VALUE&0X7FFF))+1))&0XFFFFFFFF;
    }
    else 
        new_output = (BASE + OFFSET_VALUE)&0XFFFFFFFF;

    // printf("%x %x %x\n", BASE , OFFSET_VALUE,output );

    if (~(check_over_flow && (((BASE + OFFSET_VALUE)>0XFFFFFFFF) || (BASE-((0X7FFF- (OFFSET_VALUE&0X7FFF))+1))>0XFFFFFFFF ) ))
        *output=new_output;
}


void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */

    int32_t current_code=0x0;
    int OP =0x0;
    int RS =0x0;
    int RT =0x0;
    int RD =0x0;
    int SA =0x0;
    int32_t IMMEDIATE=0x0;
    int32_t OFFSET=0x0;
    
    int16_t buffer_value= 0x0; 

    //fetch
    current_code = mem_read_32(CURRENT_STATE.PC);
    OP = (current_code&0XFC000000)>>(MIPS_REGS-6);
    NEXT_STATE.PC = CURRENT_STATE.PC+4;

    // decode RS RT RD IMMEDIATE
    RS = (current_code&0x03e00000)>>21;
    RT = (current_code&0x001f0000)>>16;
    RD = (current_code&0x0000f800)>>11;
    SA = (current_code&0x000007C0)>>6;
    
    IMMEDIATE = current_code&0X0000ffff;
    
    // printf("OP:%x   %x\n", OP, current_code);
    //decode
    if (current_code == 0b001100 && CURRENT_STATE.REGS[2] == 0xa)
        // syscall check $V0 == 0XA
        RUN_BIT=FALSE;
    
    switch (OP){

        case 0b001001:
            // addiu
            offset_val_with_sign(CURRENT_STATE.REGS[RS], IMMEDIATE&0X8000|IMMEDIATE&0X7FFF, &NEXT_STATE.REGS[RT], 0);
            break;

        case 0b001000:
            // addi
            offset_val_with_sign(CURRENT_STATE.REGS[RS], IMMEDIATE&0X8000|IMMEDIATE&0X7FFF,&NEXT_STATE.REGS[RT], 1) ;
            break;

        case 0b001110:
            // xori
            NEXT_STATE.REGS[RT] = CURRENT_STATE.REGS[RS]^IMMEDIATE;
            break;

        case 0b001100:
            // andi
            NEXT_STATE.REGS[RT] = CURRENT_STATE.REGS[RS]&IMMEDIATE;
            break;

        case 0b001111:
            // lui, load upper immediate
            NEXT_STATE.REGS[RT] = IMMEDIATE<<16;
            break;

        case 0b000010:
            // j, jump
            NEXT_STATE.PC = (NEXT_STATE.PC&0xf0000000)|((current_code<<2)&0x0ffffffc);
            break;

        case 0b000011:
            // jal, jump and link
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC+4;
            NEXT_STATE.PC = (NEXT_STATE.PC&0xf0000000)|((current_code<<2)&0x0fffffff);
            break;

        case 0b001101:
            // ori
            NEXT_STATE.REGS[RT]=(CURRENT_STATE.REGS[RS]&0xffff0000)|((CURRENT_STATE.REGS[RS]&0xffff)|IMMEDIATE);
            break;

        case 0b000101:
            // bne, branch not equal
            OFFSET = (IMMEDIATE&0X8000)|(IMMEDIATE&0X1FFF)<<2;

            if (CURRENT_STATE.REGS[RS] != CURRENT_STATE.REGS[RT])
                offset_val_with_sign(CURRENT_STATE.PC, OFFSET, &NEXT_STATE.PC, 0);
            break;

        case 0b000100:
            // beq , branch on equal
            OFFSET = (IMMEDIATE&0X8000)|(IMMEDIATE&0X1FFF)<<2;

            if (CURRENT_STATE.REGS[RS] == CURRENT_STATE.REGS[RT])
                offset_val_with_sign(CURRENT_STATE.PC, OFFSET, &NEXT_STATE.PC, 0);
            break;

        case 0b000001:
            if (RT == 0b00001)
            {
                // bgez, branch on greater than or equal ro zero
                OFFSET = (IMMEDIATE&0X8000)|(IMMEDIATE&0X1FFF)<<2;

                if ((CURRENT_STATE.REGS[RS]>>31) == 0)
                    offset_val_with_sign(CURRENT_STATE.PC, OFFSET, &NEXT_STATE.PC, 0) ;
            }
            else if (RT == 0b10001)
             {
                // bgezal, branch on greater than or equal zero and link
                OFFSET = (IMMEDIATE&0X8000)|(IMMEDIATE&0X1FFF)<<2;
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC+4;

                if ((CURRENT_STATE.REGS[RS]>>31) == 0)
                    offset_val_with_sign(CURRENT_STATE.PC, OFFSET, &NEXT_STATE.PC, 0);
            }
            else if (RT == 0b00000)
             {
                // bltz, branch on less than zero
                OFFSET = (IMMEDIATE&0X8000)|(IMMEDIATE&0X1FFF)<<2;

                if ((CURRENT_STATE.REGS[RS]>>31) == 1)
                    offset_val_with_sign(CURRENT_STATE.PC, OFFSET, &NEXT_STATE.PC, 0);
            }
            else if (RT == 0b10000)
             {
                // bltzal, branch on less than zero and link
                OFFSET = (IMMEDIATE&0X8000)|(IMMEDIATE&0X1FFF)<<2;
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC+4;

                if ((CURRENT_STATE.REGS[RS]>>31) == 1)
                    offset_val_with_sign(CURRENT_STATE.PC, OFFSET, &NEXT_STATE.PC, 0);
            }

            break;

        case 0b000111:
            // bgtz, branch on greater than zero
            OFFSET = (IMMEDIATE&0X8000)|(IMMEDIATE&0X1FFF)<<2;
            if ((CURRENT_STATE.REGS[RS]>>31) == 0 && CURRENT_STATE.REGS[RS] != 0)
                offset_val_with_sign(CURRENT_STATE.PC, OFFSET, &NEXT_STATE.PC, 0);

            break;

        case 0b000110:
            // blez, branch on less than or equal ro zero
            OFFSET = (IMMEDIATE&0X8000)|(IMMEDIATE&0X1FFF)<<2;

            if ((CURRENT_STATE.REGS[RS]>>31) == 1 || CURRENT_STATE.REGS[RS] ==0 )
                offset_val_with_sign(CURRENT_STATE.PC, OFFSET, &NEXT_STATE.PC, 0);

            break;

        case 0b101011:
            // sw, store word
            OFFSET=IMMEDIATE;
            offset_val_with_sign(CURRENT_STATE.REGS[RS], OFFSET, &buffer_value, 0);
            if (buffer_value&0X3 ==0)
                mem_write_32(buffer_value, CURRENT_STATE.REGS[RT]&0xFFFFFFFF  ); // RS is base
            break;

        case 0b101000:
            // sb, store byte
            OFFSET=IMMEDIATE;
            offset_val_with_sign(CURRENT_STATE.REGS[RS], OFFSET, &buffer_value, 0);
            mem_write_32(buffer_value, (0XFFFFFFFF00*CURRENT_STATE.REGS[RT]&0X80>>7)|CURRENT_STATE.REGS[RT]&0XFF);
            break;
            
        case 0b101001:
            // sh, store halfword  
            OFFSET=IMMEDIATE;       
            offset_val_with_sign(CURRENT_STATE.REGS[RS], OFFSET, &buffer_value, 0);
            if (buffer_value&0X1 ==0)
                mem_write_32(buffer_value, (0XFFFFFF0000*CURRENT_STATE.REGS[RT]&0X8000>>15)|CURRENT_STATE.REGS[RT]&0XFFFF);
            break;

        case 0b100011:
            // lw, load word
            OFFSET=IMMEDIATE;
            offset_val_with_sign(CURRENT_STATE.REGS[RS], OFFSET, &buffer_value, 0);
            if (buffer_value&0X3 ==0)
                NEXT_STATE.REGS[RT] = mem_read_32(buffer_value); // RS is base
            break;

        case 0b100000:
            // lb, load byte
            OFFSET=IMMEDIATE;
            offset_val_with_sign(CURRENT_STATE.REGS[RS], OFFSET, &buffer_value, 0);
            NEXT_STATE.REGS[RT] = (0XFFFFFF00*((mem_read_32(buffer_value)&0X80)>>7)) |(mem_read_32(buffer_value)&0XFF); // RS is base
            break;

        case 0b100100:
            // lbu, load byte unsign

            OFFSET=IMMEDIATE;
            offset_val_with_sign(CURRENT_STATE.REGS[RS], OFFSET, &buffer_value, 0);
            NEXT_STATE.REGS[RT] = (mem_read_32(buffer_value)&0XFF); // RS is base
            break;

        case 0b100001:
            // lh, load halfword
            OFFSET=IMMEDIATE;
            offset_val_with_sign(CURRENT_STATE.REGS[RS], OFFSET, &buffer_value, 0);
            if (buffer_value&0X1 ==0)
                NEXT_STATE.REGS[RT] = (0XFFFF0000*((mem_read_32(buffer_value)&0X80)>>7)) |(mem_read_32(buffer_value)&0XFFFF); // RS is base
            break;

        case 0b100101:
            // lhu, load halfword unsign
            OFFSET=IMMEDIATE;
            offset_val_with_sign(CURRENT_STATE.REGS[RS], OFFSET, &buffer_value, 0);
            if (buffer_value&0X1 ==0)
                NEXT_STATE.REGS[RT] = mem_read_32(buffer_value)&0XFFFF; // RS is base
            break;

        case 0b001010:
            // slti, set on less than immediate
            NEXT_STATE.REGS[RT] = 0X0<<31;
            if (CURRENT_STATE.REGS[RS]< IMMEDIATE )
                NEXT_STATE.REGS[RT] += 1 ; 
            break;
        case 0b001011:
            // sltiu, set on less than immediate unsignedn
            NEXT_STATE.REGS[RT] = 0X0<<31;
            if (CURRENT_STATE.REGS[RS]< IMMEDIATE )
                NEXT_STATE.REGS[RT] += 1 ; 
            break;



        case 0b000000:
            //SPECIAL OP
            if ( (current_code&0x3f)==0b100001)
            {
                // addu
                offset_val_with_sign(CURRENT_STATE.REGS[RS], CURRENT_STATE.REGS[RT], &NEXT_STATE.REGS[RD], 0);
            }

            else if ((current_code&0x3f)==0b100101)
            {
                // or
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RS]|CURRENT_STATE.REGS[RT];
            }

            else if ((current_code&0x3f)==0b100111)
            {
                // nor
                NEXT_STATE.REGS[RD] = (~(CURRENT_STATE.REGS[RS]|CURRENT_STATE.REGS[RT]))&0XFFFFFFFF;
            }

            else if ((current_code&0x3f)==0b100000)
            {
                // add
                // if ((((CURRENT_STATE.REGS[RS]+CURRENT_STATE.REGS[RT])>>30)&0xF)!=((CURRENT_STATE.REGS[RS]+CURRENT_STATE.REGS[RT])>>31))
                offset_val_with_sign(CURRENT_STATE.REGS[RS], CURRENT_STATE.REGS[RT], &CURRENT_STATE.REGS[RD], 1) ;
            }
            else if ((current_code&0x3f)==0b100011)
            {
                //subu
                offset_val_with_sign(CURRENT_STATE.REGS[RS], (~(CURRENT_STATE.REGS[RT]-1))&0XFFFFFFFF, &CURRENT_STATE.REGS[RD], 0); 
                
            }
            else if ((current_code&0x3f)==0b100110)
            {
                //xor
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RS]^CURRENT_STATE.REGS[RT];
            }

            else if ((current_code&0x3f)==0b000000)
            {
                //SLL, shift left n bit
                NEXT_STATE.REGS[RD] = (CURRENT_STATE.REGS[RT]<<SA)&0XFFFFFFFF;
            }
            else if ((current_code&0x3f)==0b0000010)
            {
                //SRL , shift right n bit
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT]>>SA;
            }
            else if ((current_code&0x3f)==0b0000011)
            {
                //SRA , shift right arthimetic n bit
                // NEXT_STATE.REGS[RD] = (CURRENT_STATE.REGS[RT]<<(MIPS_REGS-CURRENT_STATE.REGS[SA]))|(CURRENT_STATE.REGS[RT]>>CURRENT_STATE.REGS[SA]);
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT]<<(MIPS_REGS-SA) | (CURRENT_STATE.REGS[RT]>>SA) ;
            }
            else if ((current_code&0x3f)==0b010100)
            {
                // sllv, shift left logical variable
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT]<<CURRENT_STATE.REGS[RS];
            }
            else if ((current_code&0x3f)==0b000110)
            {
                // srlv, shift right logical variable
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT]>>CURRENT_STATE.REGS[RS];
            }
            else if ((current_code&0x3f)==0b000111)
            {
                // srav, shift right arithmetic variable
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT]<<(MIPS_REGS-CURRENT_STATE.REGS[RS]) | (CURRENT_STATE.REGS[RT]>>CURRENT_STATE.REGS[RS]) ;
            }

            else if ((current_code&0x3f)==0b100100)
            {
                // sub
                offset_val_with_sign(CURRENT_STATE.REGS[RS], (~(CURRENT_STATE.REGS[RT]-1))&0XFFFFFFFF, &NEXT_STATE.REGS[RD], 1); 
                    
            }
            else if ((current_code&0x3f)==0b001000)
            {
                // JR, jump register
                NEXT_STATE.PC = CURRENT_STATE.REGS[RS];
            }
            else if ((current_code&0x3f)==0b010001)
            {
                // JALR, jump and link register
                NEXT_STATE.REGS[RD] = CURRENT_STATE.PC+4;
                NEXT_STATE.PC = CURRENT_STATE.REGS[RS];
            }

            else if ((current_code&0x3f)==0b010000)
            {
                // MFHI, move from hi
                NEXT_STATE.REGS[RD]=CURRENT_STATE.HI; 

            }
            else if ((current_code&0x3f)==0b010010)
            {
                // MFLO, move from lo
                NEXT_STATE.REGS[RD]=CURRENT_STATE.LO;

            }
            else if ((current_code&0x3f)==0b010001)
            {
                // MTHI, move to hi
                NEXT_STATE.HI = CURRENT_STATE.REGS[RS];
                
            }
            else if ((current_code&0x3f)==0b010011)
            {
                // MTLO, move to lo
                NEXT_STATE.LO = CURRENT_STATE.REGS[RS];
                
            }
            
            else if ((current_code&0x3f)==0b011000)
            {
                //MULT , multiply
                NEXT_STATE.LO = (CURRENT_STATE.REGS[RS]* CURRENT_STATE.REGS[RT])&0XFFFFFFFF;
                NEXT_STATE.HI = (CURRENT_STATE.REGS[RS]* CURRENT_STATE.REGS[RT])&0XFFFFFFFF00000000>>32;
            }
            else if ((current_code&0x3f)==0b011001)
            {
                // MULTU, multiply unsign

                NEXT_STATE.LO = ((0<<32|CURRENT_STATE.REGS[RS])* (0<<32|CURRENT_STATE.REGS[RT]))&0XFFFFFFFF;
                NEXT_STATE.HI = (((0<<32|CURRENT_STATE.REGS[RS])* (0<<32|CURRENT_STATE.REGS[RT]))&0XFFFFFFFF00000000)>>32;
            }

            else if ((current_code&0x3f)==0b011010)
            {
                // DIV, divide 
                if (CURRENT_STATE.REGS[RT] !=0){
                    NEXT_STATE.LO = (CURRENT_STATE.REGS[RS]/ CURRENT_STATE.REGS[RT]);
                    NEXT_STATE.HI = (CURRENT_STATE.REGS[RS]% CURRENT_STATE.REGS[RT]);
                }
                else {
                    NEXT_STATE.LO=NULL;
                    NEXT_STATE.HI=NULL;
                }
            }
            else if ((current_code&0x3f)==0b011011)
            {
                // DIVU, divide unsign 
                if (CURRENT_STATE.REGS[RT] !=0){
                    NEXT_STATE.LO = ((0<<63|CURRENT_STATE.REGS[RS])/ (0<<63|CURRENT_STATE.REGS[RT]));
                    NEXT_STATE.HI = ((0<<63|CURRENT_STATE.REGS[RS])% (0<<63|CURRENT_STATE.REGS[RT]));
                }
                else {
                    NEXT_STATE.LO=NULL;
                    NEXT_STATE.HI=NULL;
                }
            }
            else if ((current_code&0x3f)==0b101011)
            {
                // sltu, set on less than unsigned
                NEXT_STATE.REGS[RD] = 0X0<<31;
                if ((unsigned)CURRENT_STATE.REGS[RS]< (unsigned)CURRENT_STATE.REGS[RT] )
                    NEXT_STATE.REGS[RD] += 1 ; 
            }
            else if ((current_code&0x3f)==0b101010)
            {
                // slt, set on less than
                NEXT_STATE.REGS[RD] = 0X0<<31;
                if (CURRENT_STATE.REGS[RS]< CURRENT_STATE.REGS[RT] )
                    NEXT_STATE.REGS[RD] += 1 ; 
            }
            break;
    }

}

// not implement yet
// slti, sltiu, sllv, srlv, srav, slt, sltu


//check integeroverflow exception on unsign operand

//overflow bit >> 1 