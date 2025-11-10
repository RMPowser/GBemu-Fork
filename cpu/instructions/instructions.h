#include "flag_ops.h"
#include "instruction.h"
#include "operand.inc"
#include "operator.inc"

namespace instruction {
using NOP = Instruction<Operator::NOP>;
using HALT = Instruction<Operator::HALT>;
using STOP = Instruction<Operator::STOP>;  // 0 cycles - enters low power mode
using DI = Instruction<Operator::DI>;
using EI = Instruction<Operator::EI>;

// 8-bit Load Instructions
using LD_R8_R8 = Instruction<Operator::LD, Operand::R8_53, Operand::R8_20>;
using LD_R8_IMM8 = Instruction<Operator::LD, Operand::R8_53, Operand::IMM8>;
using LD_R8_HLMEM = Instruction<Operator::LD, Operand::R8_53, Operand::HLMEM>;
using LD_HLMEM_R8 = Instruction<Operator::LD, Operand::HLMEM, Operand::R8_20>;
using LD_HLMEM_IMM8 = Instruction<Operator::LD, Operand::HLMEM, Operand::IMM8>;
using LD_A_R16MEM = Instruction<Operator::LD, Operand::A, Operand::R16MEM_54>;
using LD_R16MEM_A = Instruction<Operator::LD, Operand::R16MEM_54, Operand::A>;
using LD_A_BCMEM = Instruction<Operator::LD, Operand::A, Operand::BCMEM>;
using LD_A_DEMEM = Instruction<Operator::LD, Operand::A, Operand::DEMEM>;
using LD_A_HLMEM_INC = Instruction<Operator::LD, Operand::A, Operand::HLMEM_INC>;
using LD_A_HLMEM_DEC = Instruction<Operator::LD, Operand::A, Operand::HLMEM_DEC>;
using LD_BCMEM_A = Instruction<Operator::LD, Operand::BCMEM, Operand::A>;
using LD_DEMEM_A = Instruction<Operator::LD, Operand::DEMEM, Operand::A>;
using LD_HLMEM_INC_A = Instruction<Operator::LD, Operand::HLMEM_INC, Operand::A>;
using LD_HLMEM_DEC_A = Instruction<Operator::LD, Operand::HLMEM_DEC, Operand::A>;
using LD_A_IMM16MEM = Instruction<Operator::LD, Operand::A, Operand::IMM16MEM>;
using LD_IMM16MEM_A = Instruction<Operator::LD, Operand::IMM16MEM, Operand::A>;
using LDH_A_IMM8MEM = Instruction<Operator::LD, Operand::A, Operand::IMM8MEM>;
using LDH_IMM8MEM_A = Instruction<Operator::LD, Operand::IMM8MEM, Operand::A>;
using LD_A_CMEM = Instruction<Operator::LD, Operand::A, Operand::CMEM>;
using LD_CMEM_A = Instruction<Operator::LD, Operand::CMEM, Operand::A>;

// 16-bit Load Instructions
using LD_R16_IMM16 = Instruction<Operator::LD, Operand::R16_54, Operand::IMM16>;
using LD_IMM16MEM_SP = Instruction<Operator::LD, Operand::IMM16MEM, Operand::SP>;
using LD_SP_HL = Instruction<Operator::LD, Operand::SP, Operand::HL>;
using LD_HL_SP_IMM8 = Instruction<Operator::LD_HL_SP, Operand::IMM8>;
using PUSH_R16STK = Instruction<Operator::PUSH, Operand::R16STK_54>;
using POP_R16STK = Instruction<Operator::POP, Operand::R16STK_54>;

// 8-bit Arithmetic/Logic
using ADD_A_R8 = Instruction<Operator::ADD, Operand::A, Operand::R8_20, FlagOps::ADD8FLAGS>;
using ADD_A_HLMEM = Instruction<Operator::ADD, Operand::A, Operand::HLMEM, FlagOps::ADD8FLAGS>;
using ADD_A_IMM8 = Instruction<Operator::ADD, Operand::A, Operand::IMM8, FlagOps::ADD8FLAGS>;
using ADC_A_R8 = Instruction<Operator::ADC, Operand::A, Operand::R8_20>;
using ADC_A_HLMEM = Instruction<Operator::ADC, Operand::A, Operand::HLMEM>;
using ADC_A_IMM8 = Instruction<Operator::ADC, Operand::A, Operand::IMM8>;
using SUB_R8 = Instruction<Operator::SUB, Operand::R8_20>;
using SUB_HLMEM = Instruction<Operator::SUB, Operand::HLMEM>;
using SUB_IMM8 = Instruction<Operator::SUB, Operand::IMM8>;
using SBC_A_R8 = Instruction<Operator::SBC, Operand::A, Operand::R8_20>;
using SBC_A_HLMEM = Instruction<Operator::SBC, Operand::A, Operand::HLMEM>;
using SBC_A_IMM8 = Instruction<Operator::SBC, Operand::A, Operand::IMM8>;
using AND_R8 = Instruction<Operator::AND, Operand::R8_20>;
using AND_HLMEM = Instruction<Operator::AND, Operand::HLMEM>;
using AND_IMM8 = Instruction<Operator::AND, Operand::IMM8>;
using XOR_R8 = Instruction<Operator::XOR, Operand::R8_20>;
using XOR_HLMEM = Instruction<Operator::XOR, Operand::HLMEM>;
using XOR_IMM8 = Instruction<Operator::XOR, Operand::IMM8>;
using OR_R8 = Instruction<Operator::OR, Operand::R8_20>;
using OR_HLMEM = Instruction<Operator::OR, Operand::HLMEM>;
using OR_IMM8 = Instruction<Operator::OR, Operand::IMM8>;
using CP_R8 = Instruction<Operator::CP, Operand::R8_20>;
using CP_HLMEM = Instruction<Operator::CP, Operand::HLMEM>;
using CP_IMM8 = Instruction<Operator::CP, Operand::IMM8>;
using INC_R8 = Instruction<Operator::INC, Operand::R8_53, FlagOps::SET>;
using INC_HLMEM = Instruction<Operator::INC, Operand::HLMEM, FlagOps::SET>;
using DEC_R8 = Instruction<Operator::DEC, Operand::R8_53, FlagOps::SET>;
using DEC_HLMEM = Instruction<Operator::DEC, Operand::HLMEM, FlagOps::SET>;
using DAA = Instruction<Operator::DAA>;
using CPL = Instruction<Operator::CPL>;

// 16-bit Arithmetic
using ADD_HL_R16 = Instruction<Operator::ADD, Operand::HL, Operand::R16_54, FlagOps::ADD16FLAGS>;
using ADD_SP_IMM8 = Instruction<Operator::ADD_SP, Operand::SP, Operand::IMM8>;
using INC_R16 = Instruction<Operator::INC, Operand::R16_54, FlagOps::NONE>;
using DEC_R16 = Instruction<Operator::DEC, Operand::R16_54, FlagOps::NONE>;

// Rotate and Shift
using RLCA = Instruction<Operator::RLCA>;
using RLA = Instruction<Operator::RLA>;
using RRCA = Instruction<Operator::RRCA>;
using RRA = Instruction<Operator::RRA>;
using RLC_R8 = Instruction<Operator::RLC, Operand::R8_20>;
using RLC_HLMEM = Instruction<Operator::RLC, Operand::HLMEM>;
using RL_R8 = Instruction<Operator::RL, Operand::R8_20>;
using RL_HLMEM = Instruction<Operator::RL, Operand::HLMEM>;
using RRC_R8 = Instruction<Operator::RRC, Operand::R8_20>;
using RRC_HLMEM = Instruction<Operator::RRC, Operand::HLMEM>;
using RR_R8 = Instruction<Operator::RR, Operand::R8_20>;
using RR_HLMEM = Instruction<Operator::RR, Operand::HLMEM>;
using SLA_R8 = Instruction<Operator::SLA, Operand::R8_20>;
using SLA_HLMEM = Instruction<Operator::SLA, Operand::HLMEM>;
using SRA_R8 = Instruction<Operator::SRA, Operand::R8_20>;
using SRA_HLMEM = Instruction<Operator::SRA, Operand::HLMEM>;
using SRL_R8 = Instruction<Operator::SRL, Operand::R8_20>;
using SRL_HLMEM = Instruction<Operator::SRL, Operand::HLMEM>;
using SWAP_R8 = Instruction<Operator::SWAP, Operand::R8_20>;
using SWAP_HLMEM = Instruction<Operator::SWAP, Operand::HLMEM>;

// Bit Operations
using BIT_B3_R8 = Instruction<Operator::BIT, Operand::B3_53, Operand::R8_20>;
using BIT_B3_HLMEM = Instruction<Operator::BIT, Operand::B3_53, Operand::HLMEM>;
using RES_B3_R8 = Instruction<Operator::RES, Operand::B3_53, Operand::R8_20>;
using RES_B3_HLMEM = Instruction<Operator::RES, Operand::B3_53, Operand::HLMEM>;
using SET_B3_R8 = Instruction<Operator::SET, Operand::B3_53, Operand::R8_20>;
using SET_B3_HLMEM = Instruction<Operator::SET, Operand::B3_53, Operand::HLMEM>;

// Control Flow and Carry Flag
using CCF = Instruction<Operator::CCF>;
using SCF = Instruction<Operator::SCF>;

using JP_IMM16 = Instruction<Operator::JP, Operand::IMM16>;
using JP_HL = Instruction<Operator::JP, Operand::HL>;
using JP_COND_IMM16 = Instruction<Operator::JP_COND, Operand::COND_43, Operand::IMM16>;
using JR_IMM8 = Instruction<Operator::JR, Operand::IMM8>;
using JR_COND_IMM8 = Instruction<Operator::JR_COND, Operand::COND_43, Operand::IMM8>;

// Calls and Returns
using CALL_IMM16 = Instruction<Operator::CALL, Operand::IMM16>;
using CALL_COND_IMM16 = Instruction<Operator::CALL_COND, Operand::COND_43, Operand::IMM16>;
using RET = Instruction<Operator::RET>;
using RET_COND = Instruction<Operator::RET_COND, Operand::COND_43>;
using RETI = Instruction<Operator::RETI>;
using RST_TGT3 = Instruction<Operator::RST, Operand::TGT3_53>;

}  // namespace instruction