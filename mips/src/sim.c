#include <stdio.h>
#include "shell.h"
// #include <string.h>

// 1. Fetch
// 2. Decode
// 3. Execute
// 4. Memory

uint32_t INSCODE = 0;   // Instruction code
uint32_t OPCODE = 0;    // Operation code : 31-26
                                                                              
uint32_t RS;            // Decoded RS operand : 25-21       : 0x03E00000    < 00000011111000000000000000000000
uint32_t RT;            // Decoded RT operand : 20-16       : 0x001F0000    < 00000000000111110000000000000000
uint32_t RD;            // Decoded RD operand : 15-11       : 0x0000F800    < 00000000000000001111100000000000
uint32_t SHAMT;         // Decoded Shift Amount : 10-6      : 0x000007C0    < 00000000000000000000011111000000
uint32_t IMM;           // Decode Immediate Value : 15-0    : 0x0000FFFF    < 00000000000000001111111111111111
uint32_t TARGET;        // Decode Target Address : 25-0     : 0x03FFFFFF    < 00000011111111111111111111111111
uint32_t FUNCT;         // Decode Function : 5-0            : 0x0000003F    < 00000000000000000000000000111111         
                                                            
// uint32_t CURR;       // Decode Current Code

int SE_IMM;             // Decoded sign-extended immediate value
int64_t tmp;

// SPECIAL function (OP=0)
#define OP_SPE_SLL     0x0       // SLL      000000
#define OP_SPE_SRL     0x2       // SRL      000010
#define OP_SPE_SRA     0x3       // SRA      000011
#define OP_SPE_SLLV    0x4       // SLLV     000100
#define OP_SPE_SRLV    0x6       // SRLV     000110
#define OP_SPE_SRAV    0x7       // SRAV     000111
#define OP_SPE_JR      0x8       // JR       001000
#define OP_SPE_JALR    0x9       // JALR     001001
#define OP_SPE_SYSCALL 0xc       // SYSCALL  001100
#define OP_SPE_MFHI    0x10      // MFHI     010000
#define OP_SPE_MTHI    0x11      // MTHI     010001
#define OP_SPE_MFLO    0x12      // MFLO     010010
#define OP_SPE_MTLO    0x13      // MTLO     010011
#define OP_SPE_MULT    0x18      // MULT     011000
#define OP_SPE_MULTU   0x19      // MULTU    011001
#define OP_SPE_DIV     0x1a      // DIV      011010
#define OP_SPE_DIVU    0x1b      // DIVU     011011
#define OP_SPE_ADD     0x20      // ADD      100000
#define OP_SPE_ADDU    0x21      // ADDU     100001
#define OP_SPE_SUB     0x22      // SUB      100010
#define OP_SPE_SUBU    0x23      // SUBU     100011
#define OP_SPE_AND     0x24      // AND      100100
#define OP_SPE_OR      0x25      // OR       100101
#define OP_SPE_XOR     0x26      // XOR      100110
#define OP_SPE_NOR     0x27      // NOR      100111
#define OP_SPE_SLT     0x2A      // SLT      101010
#define OP_SPE_SLTU    0x2B      // SLTU     101011


// REGIMM rt (OP=1) 
#define OP_REG_BLTZ    0x0       // BLTZ     00000
#define OP_REG_BGEZ    0x1       // BGEZ     00001
#define OP_REG_BLTZL   0x2       // BLTZL    00010
#define OP_REG_BGEZL   0x3       // BGEZL    00011
#define OP_REG_TGEI    0x8       // TGEI     01000
#define OP_REG_TGEIU   0x9       // TGEIU    01001
#define OP_REG_TLTI    0xA       // TLTI     01010
#define OP_REG_TLTIU   0xB       // TLTIU    01011
#define OP_REG_TEQI    0xC       // TEQI     01100
#define OP_REG_TNEI    0xE       // TNEI     01110
#define OP_REG_BLTZAL  0x10      // BLTZAL   10000
#define OP_REG_BGEZAL  0x11      // BGEZAL   10001
#define OP_REG_BLTZALL 0x12      // BLTZALL  10010
#define OP_REG_BGEZALL 0x13      // BGEZALL  10011

// Opcodes
#define OP_SPECIAL     0x00      // SPECIAL  000000
#define OP_SPECIAL_REG 0x01      // Branch On Greater Than
#define OP_J           0x02      // J        000010
#define OP_JAL         0x03      // JAL      000011
#define OP_BEQ         0x04      // BEQ      000100
#define OP_BNE         0x05      // BNE      000101
#define OP_BLEZ        0x06      // BLEZ     000110
#define OP_BGTZ        0x07      // BGTZ     000111
#define OP_ADDI        0x08      // ADDI     001000
#define OP_ADDIU       0x09      // ADDIU    001001
#define OP_SLTI        0x0a      // SLTI     001010
#define OP_SLTIU       0x0b      // SLTIU    001011
#define OP_ANDI        0x0c      // ANDI     001100
#define OP_ORI         0x0d      // ORI      001101
#define OP_XORI        0x0e      // XORI     001110
#define OP_LUI         0x0f      // LUI      001111
#define OP_LB          0x20      // LB       100000
#define OP_LH          0x21      // LH       100001
#define OP_LW          0x23      // LW       100011
#define OP_LBU         0x24      // LBU      100100
#define OP_LHU         0x25      // LHU      100101
#define OP_SB          0x28      // SB       101000
#define OP_SH          0x29      // SH       101001
#define OP_SW          0x2b      // SW       101011

uint32_t sign_extend_b2w(uint8_t c)
{
    // return (c & 0x80) ? (c | 0xffffff80) : c;
    // return condition? returnValue1 : returnValue2;
    // 0xffffff80 = 11111111111111111111111110000000, 0x80 = 10000000

    if (c & 0x80) {
        return (c | 0xffffff80);
    } else {
        return c;
    }
}

uint32_t sign_extend_h2w(uint16_t c)
{
    return (c & 0x8000) ? (c | 0xffff8000) : c;
}

uint32_t zero_extend_b2w(uint8_t c)
{
    return ((uint32_t)c);
}

uint32_t zero_extend_h2w(uint16_t c)
{
    return ((uint32_t)c);
}

int sign_extend_18b()
{
    return (IMM << 2);
}

void process_instruction()
{

    // fetch the 4 bytes of the current instruction
    INSCODE = mem_read_32(CURRENT_STATE.PC);
    NEXT_STATE.PC = CURRENT_STATE.PC+4;

    OPCODE = (INSCODE&0x0000003F)>>26;
    RS = (INSCODE&0x03E00000)>>21;
    RT = (INSCODE&0x001F0000)>>16;
    RD = (INSCODE&0x0000F800)>>11;
    SHAMT = (INSCODE&0x000007C0)>>6;
    IMM = (INSCODE&0x0000FFFF)>>0;
    SE_IMM = (INSCODE&0x0000FFFF)>>0;
    TARGET = (INSCODE>>0)&((1UL << 26) - 1); //is an unsigned long version of 1
    FUNCT = (INSCODE&0x0000003F)>>0;
    SE_IMM = sign_extend_h2w(IMM);
    // decode an instruction into fields

    // update architectural state according to instruction
    switch (OPCODE)
    {
    case OP_SPECIAL:
    {
        switch (FUNCT)
        {
        case OP_SPE_ADD:
        case OP_SPE_ADDU:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RS] + CURRENT_STATE.REGS[RT];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        case OP_SPE_SYSCALL:
            if (CURRENT_STATE.REGS[2] == 10)
                RUN_BIT = 0;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
            // specify the remaining FUNCT cases below this line
        //AND Operation
        case OP_SPE_AND:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RS] & CURRENT_STATE.REGS[RT];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

            //DIV Operation
        case OP_SPE_DIV:
            if (RT != 0)
            {
                NEXT_STATE.HI = (int)CURRENT_STATE.REGS[RS] % (int)CURRENT_STATE.REGS[RT];
                NEXT_STATE.LO = (int)CURRENT_STATE.REGS[RS] / (int)CURRENT_STATE.REGS[RT];
            }
            else
                printf("Division by zero. Value unpredictable");
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

            //Unsigned DIV Operation
        case OP_SPE_DIVU:
            if (RT != 0)
            {
                NEXT_STATE.HI = CURRENT_STATE.REGS[RS] % CURRENT_STATE.REGS[RT];
                NEXT_STATE.LO = CURRENT_STATE.REGS[RS] / CURRENT_STATE.REGS[RT];
            }
            else
                printf("Division by zero. Value unpredictable");
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

            //Jump and Link Register
        case OP_SPE_JALR:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.PC + 8;
            if (!(CURRENT_STATE.REGS[RS] & 0x03)) // 00000011
                NEXT_STATE.PC = CURRENT_STATE.REGS[RS];
            else
                printf(" Address not word aligned ");
            break;

        //Jump Register
        case OP_SPE_JR:
            if (!(CURRENT_STATE.REGS[RS] & 0x03)) // 00000011
                NEXT_STATE.PC = CURRENT_STATE.REGS[RS];
            else
            {
                printf(" Address not word aligned ");
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;

        //Move from High Register
        case OP_SPE_MFHI:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.HI;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Move from Low register
        case OP_SPE_MFLO:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.LO;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Move to High register
        case OP_SPE_MTHI:
            if (RS != 0)
                NEXT_STATE.HI = CURRENT_STATE.REGS[RS];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Move to Low register
        case OP_SPE_MTLO:
            if (RS != 0)
                NEXT_STATE.LO = CURRENT_STATE.REGS[RS];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Multiply opration
        case OP_SPE_MULT:
            tmp = ((int)CURRENT_STATE.REGS[RS] * (int)CURRENT_STATE.REGS[RT]);
            NEXT_STATE.HI = (tmp >> 31) & 0xFFFFFFFF;
            NEXT_STATE.LO = (tmp)&0xFFFFFFFF;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Unsigned multiply operation
        case OP_SPE_MULTU:
            NEXT_STATE.HI = ((CURRENT_STATE.REGS[RS] * CURRENT_STATE.REGS[RT]) >> 31);
            NEXT_STATE.LO = (CURRENT_STATE.REGS[RS] * CURRENT_STATE.REGS[RT]) & 0xFFFFFFFF;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //NOR operation
        case OP_SPE_NOR:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = ~(CURRENT_STATE.REGS[RS] | CURRENT_STATE.REGS[RT]);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //OR operation
        case OP_SPE_OR:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = (CURRENT_STATE.REGS[RS] | CURRENT_STATE.REGS[RT]);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //SHift Left logical
        case OP_SPE_SLL:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT] << SHAMT;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Shift Word Left Logical Variable
        case OP_SPE_SLLV:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT] << (CURRENT_STATE.REGS[RS] & 0x1F);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Set on Less than
        case OP_SPE_SLT:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = ((int)CURRENT_STATE.REGS[RS] < (int)CURRENT_STATE.REGS[RT]) ? 1 : 0;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Set on less than unsigned
        case OP_SPE_SLTU:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = (CURRENT_STATE.REGS[RS] < CURRENT_STATE.REGS[RT]) ? 1 : 0;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Shift right Arithmetic
        case OP_SPE_SRA:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = (int)CURRENT_STATE.REGS[RT] >> SHAMT;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Shift Word Right Arithmetic Variable
        case OP_SPE_SRAV:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = (int)CURRENT_STATE.REGS[RT] >> (CURRENT_STATE.REGS[RS] & 0x1F);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Shift Right Logical
        case OP_SPE_SRL:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT] >> SHAMT;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Shift Right Word logical Variable
        case OP_SPE_SRLV:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT] >> (CURRENT_STATE.REGS[RS] & 0x1F);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Subtract
        case OP_SPE_SUB:
        case OP_SPE_SUBU:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RS] - CURRENT_STATE.REGS[RT];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //XOR Operation
        case OP_SPE_XOR:
            if (RD != 0)
                NEXT_STATE.REGS[RD] = CURRENT_STATE.REGS[RT] ^ CURRENT_STATE.REGS[RS];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
            // specify the remaining FUNCT cases above this line
        default:
            printf("Encountered unimplemented subopcode (0x%x). Ending simulation...\n\n", FUNCT);
            RUN_BIT = 0;
        } // switch (FUNCT)
    }     // case OP_SPECIAL
    break;
    case OP_ADDI:
    case OP_ADDIU:
        if (RT != 0)
            NEXT_STATE.REGS[RT] = CURRENT_STATE.REGS[RS] + SE_IMM;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;
        // specify the remaining OPCODE cases below this line
    case OP_SPECIAL_REG: // special branches
    {
        switch (RT)
        {
        //Branch on Less than Zero
        case OP_REG_BLTZ:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if ((int)CURRENT_STATE.REGS[RS] < 0)
                NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
            break;

        //Branch if Greater than or equal to Zero
        case OP_REG_BGEZ:
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            if ((int)CURRENT_STATE.REGS[RS] >= 0)
                NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
            break;

        //Branch on Less Than Zero and Link
        case OP_REG_BLTZAL:
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            if ((int)CURRENT_STATE.REGS[RS] < 0)
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + sign_extend_18b();
            else
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;

        //Branch on Greater Than or Equal to Zero and Link
        case OP_REG_BGEZAL:
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            if ((int)CURRENT_STATE.REGS[RS] >= 0)
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + sign_extend_18b();
            else
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
    } // special branches
    break;

    //Set on Less Than Immediate
    case OP_SLTI:
        NEXT_STATE.REGS[RT] = (((int)CURRENT_STATE.REGS[RS] < (int)SE_IMM) ? 1 : 0);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Set on Less Than Immediate Unsigned
    case OP_SLTIU:
        NEXT_STATE.REGS[RT] = ((CURRENT_STATE.REGS[RS] < SE_IMM) ? 1 : 0);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //AND with immidiate
    case OP_ANDI:
        NEXT_STATE.REGS[RT] = (CURRENT_STATE.REGS[RS] & zero_extend_h2w(IMM));
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //OR with immidiate
    case OP_ORI:
        NEXT_STATE.REGS[RT] = (CURRENT_STATE.REGS[RS] | zero_extend_h2w(IMM));
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //XOR with immidiate
    case OP_XORI:
        NEXT_STATE.REGS[RT] = (CURRENT_STATE.REGS[RS] ^ zero_extend_h2w(IMM));
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //JUMP
    case OP_J:
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (TARGET << 2);
        // 0xF0000000 = 11110000000000000000000000000000
        break;

    //Jump and Link
    case OP_JAL:
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (TARGET << 2);
        // 0xF0000000 = 11110000000000000000000000000000
        break;

    //Branch if Equal
    case OP_BEQ:
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        if (CURRENT_STATE.REGS[RS] == CURRENT_STATE.REGS[RT])
            NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
        break;

    //Branch if not equal
    case OP_BNE:
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        if (CURRENT_STATE.REGS[RS] != CURRENT_STATE.REGS[RT])
            NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
        break;

    //Branch if less than equal to Zero
    case OP_BLEZ:
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        if (CURRENT_STATE.REGS[RS] <= 0)
            NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
        break;

    //branch if Greater than Zero
    case OP_BGTZ:
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        if (CURRENT_STATE.REGS[RS] > 0)
            NEXT_STATE.PC = NEXT_STATE.PC + sign_extend_18b();
        break;

    //Load Upper Immediate
    case OP_LUI:
        if (RS == 0)
            NEXT_STATE.REGS[RT] = (IMM << 16);
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Load Byte
    case OP_LB:
        NEXT_STATE.REGS[RT] = sign_extend_b2w((mem_read_32(CURRENT_STATE.REGS[RS] + SE_IMM)) & 0xFF);
        // 0xFF = 11111111
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Load Halfword
    case OP_LH:
        NEXT_STATE.REGS[RT] = sign_extend_h2w((mem_read_32(CURRENT_STATE.REGS[RS] + SE_IMM)) & 0xFFFF);
        // 0xFFFF = 1111111111111111
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Load Word
    case OP_LW:
        NEXT_STATE.REGS[RT] = mem_read_32((CURRENT_STATE.REGS[RS] + SE_IMM));
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Load Byte Unsigned
    case OP_LBU:
        NEXT_STATE.REGS[RT] = zero_extend_b2w((mem_read_32(CURRENT_STATE.REGS[RS] + SE_IMM)) & 0xFF);
        // 0xFF = 11111111
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Load Halfword Unsigned
    case OP_LHU:
        NEXT_STATE.REGS[RT] = zero_extend_h2w((mem_read_32(CURRENT_STATE.REGS[RS] + SE_IMM)) & 0xFFFF);
        // 0xFFFF = 1111111111111111
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Store Byte
    case OP_SB:
        mem_write_32((CURRENT_STATE.REGS[RS] + SE_IMM), (int)((CURRENT_STATE.REGS[RT]) & 0xFF));
        // 0xFF = 11111111
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Store Halfword
    case OP_SH:
        mem_write_32((CURRENT_STATE.REGS[RS] + SE_IMM), (int)((CURRENT_STATE.REGS[RT]) & 0xFFFF));
        // 0xFFFF = 1111111111111111
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

    //Store Word
    case OP_SW:
        mem_write_32((CURRENT_STATE.REGS[RS] + SE_IMM), (int)((CURRENT_STATE.REGS[RT])));
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;

        // specify the remaining OPCODE cases above this line
    default:
        printf("Encountered unimplemented opcode (0x%x). Ending simulation...\n\n", OPCODE);
        RUN_BIT = 0;
    } // switch (OPCODE)

}



// LAB1 - addiu.text.dat
// addiu $v0, $zero, 10  >> 2402000a >> /1001/0000/0000/100000000000001010
// addiu $t0, $zero, 5   >> 24080005 >> /1001/0000/0010/000000000000000101
// addiu $t1, $t0, 300   >> 2509012c >> /1001/0100/0010/010000000100101100
// addiu $t2, $zero, 500 >> 240a01f4 >> /1001/0000/0010/100000000111110100
// addiu $t3, $t2, 34    >> 254b0022 >> /1001/0101/0010/110000000000100010
// addiu $t3, $t3, 45    >> 256b002d >> /1001/0101/1010/110000000000101101
// syscall               >> 0000000c >> 1100