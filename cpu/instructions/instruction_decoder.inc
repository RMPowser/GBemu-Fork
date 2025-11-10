#include "instruction_decoder.h"

#include <iostream>
#include <tuple>
#include "cpu.h"
#include "string_utils.h"

#include "instructions.h"

template <typename Bus>
struct InstructionHandler {

  template <typename Tuple>
  struct tuple_types;

  template <typename... Ts>
  struct tuple_types<std::tuple<Ts...>> {
    template <typename Func, typename... Args>
    static decltype(auto) invoke(Func&& f, Args&&... a) {
      if constexpr (sizeof...(Ts) > 0)
        return std::forward<Func>(f).template operator()<Bus, Ts...>(std::forward<Args>(a)...);
      else
        return std::forward<Func>(f).template operator()<Bus>(std::forward<Args>(a)...);
    }
  };

  template <typename Instruction>
  static void execute(CPU<Bus>* cpu) {
    tuple_types<typename Instruction::Operands>::invoke(typename Instruction::Operator{}, cpu);
    cpu->tick();
  }

  using InstructionHandlerType = void (*)(CPU<Bus>*);

  static constexpr std::array<InstructionHandlerType, 256> opcode_array = {{
      /* 0x00 */ +execute<instruction::NOP>,
      /* 0x01 */ +execute<instruction::LD_R16_IMM16>,
      /* 0x02 */ +execute<instruction::LD_BCMEM_A>,
      /* 0x03 */ +execute<instruction::INC_R16>,
      /* 0x04 */ +execute<instruction::INC_R8>,
      /* 0x05 */ +execute<instruction::DEC_R8>,
      /* 0x06 */ +execute<instruction::LD_R8_IMM8>,
      /* 0x07 */ +execute<instruction::RLCA>,
      /* 0x08 */ +execute<instruction::LD_IMM16MEM_SP>,
      /* 0x09 */ +execute<instruction::ADD_HL_R16>,
      /* 0x0A */ +execute<instruction::LD_A_BCMEM>,
      /* 0x0B */ +execute<instruction::DEC_R16>,
      /* 0x0C */ +execute<instruction::INC_R8>,
      /* 0x0D */ +execute<instruction::DEC_R8>,
      /* 0x0E */ +execute<instruction::LD_R8_IMM8>,
      /* 0x0F */ +execute<instruction::RRCA>,
      /* 0x10 */ +execute<instruction::STOP>,
      /* 0x11 */ +execute<instruction::LD_R16_IMM16>,
      /* 0x12 */ +execute<instruction::LD_DEMEM_A>,
      /* 0x13 */ +execute<instruction::INC_R16>,
      /* 0x14 */ +execute<instruction::INC_R8>,
      /* 0x15 */ +execute<instruction::DEC_R8>,
      /* 0x16 */ +execute<instruction::LD_R8_IMM8>,
      /* 0x17 */ +execute<instruction::RLA>,
      /* 0x18 */ +execute<instruction::JR_IMM8>,
      /* 0x19 */ +execute<instruction::ADD_HL_R16>,
      /* 0x1A */ +execute<instruction::LD_A_DEMEM>,
      /* 0x1B */ +execute<instruction::DEC_R16>,
      /* 0x1C */ +execute<instruction::INC_R8>,
      /* 0x1D */ +execute<instruction::DEC_R8>,
      /* 0x1E */ +execute<instruction::LD_R8_IMM8>,
      /* 0x1F */ +execute<instruction::RRA>,
      /* 0x20 */ +execute<instruction::JR_COND_IMM8>,
      /* 0x21 */ +execute<instruction::LD_R16_IMM16>,
      /* 0x22 */ +execute<instruction::LD_HLMEM_INC_A>,
      /* 0x23 */ +execute<instruction::INC_R16>,
      /* 0x24 */ +execute<instruction::INC_R8>,
      /* 0x25 */ +execute<instruction::DEC_R8>,
      /* 0x26 */ +execute<instruction::LD_R8_IMM8>,
      /* 0x27 */ +execute<instruction::DAA>,
      /* 0x28 */ +execute<instruction::JR_COND_IMM8>,
      /* 0x29 */ +execute<instruction::ADD_HL_R16>,
      /* 0x2A */ +execute<instruction::LD_A_HLMEM_INC>,
      /* 0x2B */ +execute<instruction::DEC_R16>,
      /* 0x2C */ +execute<instruction::INC_R8>,
      /* 0x2D */ +execute<instruction::DEC_R8>,
      /* 0x2E */ +execute<instruction::LD_R8_IMM8>,
      /* 0x2F */ +execute<instruction::CPL>,
      /* 0x30 */ +execute<instruction::JR_COND_IMM8>,
      /* 0x31 */ +execute<instruction::LD_R16_IMM16>,
      /* 0x32 */ +execute<instruction::LD_HLMEM_DEC_A>,
      /* 0x33 */ +execute<instruction::INC_R16>,
      /* 0x34 */ +execute<instruction::INC_HLMEM>,
      /* 0x35 */ +execute<instruction::DEC_HLMEM>,
      /* 0x36 */ +execute<instruction::LD_HLMEM_IMM8>,
      /* 0x37 */ +execute<instruction::SCF>,
      /* 0x38 */ +execute<instruction::JR_COND_IMM8>,
      /* 0x39 */ +execute<instruction::ADD_HL_R16>,
      /* 0x3A */ +execute<instruction::LD_A_HLMEM_DEC>,
      /* 0x3B */ +execute<instruction::DEC_R16>,
      /* 0x3C */ +execute<instruction::INC_R8>,
      /* 0x3D */ +execute<instruction::DEC_R8>,
      /* 0x3E */ +execute<instruction::LD_R8_IMM8>,
      /* 0x3F */ +execute<instruction::CCF>,
      /* 0x40 */ +execute<instruction::LD_R8_R8>,
      /* 0x41 */ +execute<instruction::LD_R8_R8>,
      /* 0x42 */ +execute<instruction::LD_R8_R8>,
      /* 0x43 */ +execute<instruction::LD_R8_R8>,
      /* 0x44 */ +execute<instruction::LD_R8_R8>,
      /* 0x45 */ +execute<instruction::LD_R8_R8>,
      /* 0x46 */ +execute<instruction::LD_R8_HLMEM>,
      /* 0x47 */ +execute<instruction::LD_R8_R8>,
      /* 0x48 */ +execute<instruction::LD_R8_R8>,
      /* 0x49 */ +execute<instruction::LD_R8_R8>,
      /* 0x4A */ +execute<instruction::LD_R8_R8>,
      /* 0x4B */ +execute<instruction::LD_R8_R8>,
      /* 0x4C */ +execute<instruction::LD_R8_R8>,
      /* 0x4D */ +execute<instruction::LD_R8_R8>,
      /* 0x4E */ +execute<instruction::LD_R8_HLMEM>,
      /* 0x4F */ +execute<instruction::LD_R8_R8>,

      /* 0x50 */ +execute<instruction::LD_R8_R8>,
      /* 0x51 */ +execute<instruction::LD_R8_R8>,
      /* 0x52 */ +execute<instruction::LD_R8_R8>,
      /* 0x53 */ +execute<instruction::LD_R8_R8>,
      /* 0x54 */ +execute<instruction::LD_R8_R8>,
      /* 0x55 */ +execute<instruction::LD_R8_R8>,
      /* 0x56 */ +execute<instruction::LD_R8_HLMEM>,
      /* 0x57 */ +execute<instruction::LD_R8_R8>,
      /* 0x58 */ +execute<instruction::LD_R8_R8>,
      /* 0x59 */ +execute<instruction::LD_R8_R8>,
      /* 0x5A */ +execute<instruction::LD_R8_R8>,
      /* 0x5B */ +execute<instruction::LD_R8_R8>,
      /* 0x5C */ +execute<instruction::LD_R8_R8>,
      /* 0x5D */ +execute<instruction::LD_R8_R8>,
      /* 0x5E */ +execute<instruction::LD_R8_HLMEM>,
      /* 0x5F */ +execute<instruction::LD_R8_R8>,
      /* 0x60 */ +execute<instruction::LD_R8_R8>,
      /* 0x61 */ +execute<instruction::LD_R8_R8>,
      /* 0x62 */ +execute<instruction::LD_R8_R8>,
      /* 0x63 */ +execute<instruction::LD_R8_R8>,
      /* 0x64 */ +execute<instruction::LD_R8_R8>,
      /* 0x65 */ +execute<instruction::LD_R8_R8>,
      /* 0x66 */ +execute<instruction::LD_R8_HLMEM>,
      /* 0x67 */ +execute<instruction::LD_R8_R8>,
      /* 0x68 */ +execute<instruction::LD_R8_R8>,
      /* 0x69 */ +execute<instruction::LD_R8_R8>,
      /* 0x6A */ +execute<instruction::LD_R8_R8>,
      /* 0x6B */ +execute<instruction::LD_R8_R8>,
      /* 0x6C */ +execute<instruction::LD_R8_R8>,
      /* 0x6D */ +execute<instruction::LD_R8_R8>,
      /* 0x6E */ +execute<instruction::LD_R8_HLMEM>,
      /* 0x6F */ +execute<instruction::LD_R8_R8>,

      /* 0x70 */ +execute<instruction::LD_HLMEM_R8>,
      /* 0x71 */ +execute<instruction::LD_HLMEM_R8>,
      /* 0x72 */ +execute<instruction::LD_HLMEM_R8>,
      /* 0x73 */ +execute<instruction::LD_HLMEM_R8>,
      /* 0x74 */ +execute<instruction::LD_HLMEM_R8>,
      /* 0x75 */ +execute<instruction::LD_HLMEM_R8>,
      /* 0x76 */ +execute<instruction::HALT>,
      /* 0x77 */ +execute<instruction::LD_HLMEM_R8>,
      /* 0x78 */ +execute<instruction::LD_R8_R8>,
      /* 0x79 */ +execute<instruction::LD_R8_R8>,
      /* 0x7A */ +execute<instruction::LD_R8_R8>,
      /* 0x7B */ +execute<instruction::LD_R8_R8>,
      /* 0x7C */ +execute<instruction::LD_R8_R8>,
      /* 0x7D */ +execute<instruction::LD_R8_R8>,
      /* 0x7E */ +execute<instruction::LD_R8_HLMEM>,
      /* 0x7F */ +execute<instruction::LD_R8_R8>,
      /* 0x80 */ +execute<instruction::ADD_A_R8>,
      /* 0x81 */ +execute<instruction::ADD_A_R8>,
      /* 0x82 */ +execute<instruction::ADD_A_R8>,
      /* 0x83 */ +execute<instruction::ADD_A_R8>,
      /* 0x84 */ +execute<instruction::ADD_A_R8>,
      /* 0x85 */ +execute<instruction::ADD_A_R8>,
      /* 0x86 */ +execute<instruction::ADD_A_HLMEM>,
      /* 0x87 */ +execute<instruction::ADD_A_R8>,
      /* 0x88 */ +execute<instruction::ADC_A_R8>,
      /* 0x89 */ +execute<instruction::ADC_A_R8>,
      /* 0x8A */ +execute<instruction::ADC_A_R8>,
      /* 0x8B */ +execute<instruction::ADC_A_R8>,
      /* 0x8C */ +execute<instruction::ADC_A_R8>,
      /* 0x8D */ +execute<instruction::ADC_A_R8>,
      /* 0x8E */ +execute<instruction::ADC_A_HLMEM>,
      /* 0x8F */ +execute<instruction::ADC_A_R8>,
      /* 0x90 */ +execute<instruction::SUB_R8>,
      /* 0x91 */ +execute<instruction::SUB_R8>,
      /* 0x92 */ +execute<instruction::SUB_R8>,
      /* 0x93 */ +execute<instruction::SUB_R8>,
      /* 0x94 */ +execute<instruction::SUB_R8>,
      /* 0x95 */ +execute<instruction::SUB_R8>,
      /* 0x96 */ +execute<instruction::SUB_HLMEM>,
      /* 0x97 */ +execute<instruction::SUB_R8>,
      /* 0x98 */ +execute<instruction::SBC_A_R8>,
      /* 0x99 */ +execute<instruction::SBC_A_R8>,
      /* 0x9A */ +execute<instruction::SBC_A_R8>,
      /* 0x9B */ +execute<instruction::SBC_A_R8>,
      /* 0x9C */ +execute<instruction::SBC_A_R8>,
      /* 0x9D */ +execute<instruction::SBC_A_R8>,
      /* 0x9E */ +execute<instruction::SBC_A_HLMEM>,
      /* 0x9F */ +execute<instruction::SBC_A_R8>,
      /* 0xA0 */ +execute<instruction::AND_R8>,
      /* 0xA1 */ +execute<instruction::AND_R8>,
      /* 0xA2 */ +execute<instruction::AND_R8>,
      /* 0xA3 */ +execute<instruction::AND_R8>,
      /* 0xA4 */ +execute<instruction::AND_R8>,
      /* 0xA5 */ +execute<instruction::AND_R8>,
      /* 0xA6 */ +execute<instruction::AND_HLMEM>,
      /* 0xA7 */ +execute<instruction::AND_R8>,
      /* 0xA8 */ +execute<instruction::XOR_R8>,
      /* 0xA9 */ +execute<instruction::XOR_R8>,
      /* 0xAA */ +execute<instruction::XOR_R8>,
      /* 0xAB */ +execute<instruction::XOR_R8>,
      /* 0xAC */ +execute<instruction::XOR_R8>,
      /* 0xAD */ +execute<instruction::XOR_R8>,
      /* 0xAE */ +execute<instruction::XOR_HLMEM>,
      /* 0xAF */ +execute<instruction::XOR_R8>,
      /* 0xB0 */ +execute<instruction::OR_R8>,
      /* 0xB1 */ +execute<instruction::OR_R8>,
      /* 0xB2 */ +execute<instruction::OR_R8>,
      /* 0xB3 */ +execute<instruction::OR_R8>,
      /* 0xB4 */ +execute<instruction::OR_R8>,
      /* 0xB5 */ +execute<instruction::OR_R8>,
      /* 0xB6 */ +execute<instruction::OR_HLMEM>,
      /* 0xB7 */ +execute<instruction::OR_R8>,
      /* 0xB8 */ +execute<instruction::CP_R8>,
      /* 0xB9 */ +execute<instruction::CP_R8>,
      /* 0xBA */ +execute<instruction::CP_R8>,
      /* 0xBB */ +execute<instruction::CP_R8>,
      /* 0xBC */ +execute<instruction::CP_R8>,
      /* 0xBD */ +execute<instruction::CP_R8>,
      /* 0xBE */ +execute<instruction::CP_HLMEM>,
      /* 0xBF */ +execute<instruction::CP_R8>,

      /* 0xC0 */ +execute<instruction::RET_COND>,
      /* 0xC1 */ +execute<instruction::POP_R16STK>,
      /* 0xC2 */ +execute<instruction::JP_COND_IMM16>,
      /* 0xC3 */ +execute<instruction::JP_IMM16>,
      /* 0xC4 */ +execute<instruction::CALL_COND_IMM16>,
      /* 0xC5 */ +execute<instruction::PUSH_R16STK>,
      /* 0xC6 */ +execute<instruction::ADD_A_IMM8>,
      /* 0xC7 */ +execute<instruction::RST_TGT3>,
      /* 0xC8 */ +execute<instruction::RET_COND>,
      /* 0xC9 */ +execute<instruction::RET>,
      /* 0xCA */ +execute<instruction::JP_COND_IMM16>,
      /* 0xCB */ nullptr,
      /* 0xCC */ +execute<instruction::CALL_COND_IMM16>,
      /* 0xCD */ +execute<instruction::CALL_IMM16>,
      /* 0xCE */ +execute<instruction::ADC_A_IMM8>,
      /* 0xCF */ +execute<instruction::RST_TGT3>,

      /* 0xD0 */ +execute<instruction::RET_COND>,
      /* 0xD1 */ +execute<instruction::POP_R16STK>,
      /* 0xD2 */ +execute<instruction::JP_COND_IMM16>,
      /* 0xD3 */ nullptr,
      /* 0xD4 */ +execute<instruction::CALL_COND_IMM16>,
      /* 0xD5 */ +execute<instruction::PUSH_R16STK>,
      /* 0xD6 */ +execute<instruction::SUB_IMM8>,
      /* 0xD7 */ +execute<instruction::RST_TGT3>,
      /* 0xD8 */ +execute<instruction::RET_COND>,
      /* 0xD9 */ +execute<instruction::RETI>,
      /* 0xDA */ +execute<instruction::JP_COND_IMM16>,
      /* 0xDB */ nullptr,
      /* 0xDC */ +execute<instruction::CALL_COND_IMM16>,
      /* 0xDD */ nullptr,
      /* 0xDE */ +execute<instruction::SBC_A_IMM8>,
      /* 0xDF */ +execute<instruction::RST_TGT3>,

      /* 0xE0 */ +execute<instruction::LDH_IMM8MEM_A>,
      /* 0xE1 */ +execute<instruction::POP_R16STK>,
      /* 0xE2 */ +execute<instruction::LD_CMEM_A>,
      /* 0xE3 */ nullptr,
      /* 0xE4 */ nullptr,
      /* 0xE5 */ +execute<instruction::PUSH_R16STK>,
      /* 0xE6 */ +execute<instruction::AND_IMM8>,
      /* 0xE7 */ +execute<instruction::RST_TGT3>,
      /* 0xE8 */ +execute<instruction::ADD_SP_IMM8>,
      /* 0xE9 */ +execute<instruction::JP_HL>,
      /* 0xEA */ +execute<instruction::LD_IMM16MEM_A>,
      /* 0xEB */ nullptr,
      /* 0xEC */ nullptr,
      /* 0xED */ nullptr,
      /* 0xEE */ +execute<instruction::XOR_IMM8>,
      /* 0xEF */ +execute<instruction::RST_TGT3>,

      /* 0xF0 */ +execute<instruction::LDH_A_IMM8MEM>,
      /* 0xF1 */ +execute<instruction::POP_R16STK>,
      /* 0xF2 */ +execute<instruction::LD_A_CMEM>,
      /* 0xF3 */ +execute<instruction::DI>,
      /* 0xF4 */ nullptr,
      /* 0xF5 */ +execute<instruction::PUSH_R16STK>,
      /* 0xF6 */ +execute<instruction::OR_IMM8>,
      /* 0xF7 */ +execute<instruction::RST_TGT3>,
      /* 0xF8 */ +execute<instruction::LD_HL_SP_IMM8>,
      /* 0xF9 */ +execute<instruction::LD_SP_HL>,
      /* 0xFA */ +execute<instruction::LD_A_IMM16MEM>,
      /* 0xFB */ +execute<instruction::EI>,
      /* 0xFC */ nullptr,
      /* 0xFD */ nullptr,
      /* 0xFE */ +execute<instruction::CP_IMM8>,
      /* 0xFF */ +execute<instruction::RST_TGT3>,
  }};

  static constexpr std::array<InstructionHandlerType, 256> cb_opcode_array = {{
      /* 0x00 */ +execute<instruction::RLC_R8>,       /* 0x01 */ +execute<instruction::RLC_R8>,
      /* 0x02 */ +execute<instruction::RLC_R8>,       /* 0x03 */ +execute<instruction::RLC_R8>,
      /* 0x04 */ +execute<instruction::RLC_R8>,       /* 0x05 */ +execute<instruction::RLC_R8>,
      /* 0x06 */ +execute<instruction::RLC_HLMEM>,    /* 0x07 */ +execute<instruction::RLC_R8>,

      /* 0x08 */ +execute<instruction::RRC_R8>,       /* 0x09 */ +execute<instruction::RRC_R8>,
      /* 0x0A */ +execute<instruction::RRC_R8>,       /* 0x0B */ +execute<instruction::RRC_R8>,
      /* 0x0C */ +execute<instruction::RRC_R8>,       /* 0x0D */ +execute<instruction::RRC_R8>,
      /* 0x0E */ +execute<instruction::RRC_HLMEM>,    /* 0x0F */ +execute<instruction::RRC_R8>,

      /* 0x10 */ +execute<instruction::RL_R8>,        /* 0x11 */ +execute<instruction::RL_R8>,
      /* 0x12 */ +execute<instruction::RL_R8>,        /* 0x13 */ +execute<instruction::RL_R8>,
      /* 0x14 */ +execute<instruction::RL_R8>,        /* 0x15 */ +execute<instruction::RL_R8>,
      /* 0x16 */ +execute<instruction::RL_HLMEM>,     /* 0x17 */ +execute<instruction::RL_R8>,

      /* 0x18 */ +execute<instruction::RR_R8>,        /* 0x19 */ +execute<instruction::RR_R8>,
      /* 0x1A */ +execute<instruction::RR_R8>,        /* 0x1B */ +execute<instruction::RR_R8>,
      /* 0x1C */ +execute<instruction::RR_R8>,        /* 0x1D */ +execute<instruction::RR_R8>,
      /* 0x1E */ +execute<instruction::RR_HLMEM>,     /* 0x1F */ +execute<instruction::RR_R8>,

      /* 0x20 */ +execute<instruction::SLA_R8>,       /* 0x21 */ +execute<instruction::SLA_R8>,
      /* 0x22 */ +execute<instruction::SLA_R8>,       /* 0x23 */ +execute<instruction::SLA_R8>,
      /* 0x24 */ +execute<instruction::SLA_R8>,       /* 0x25 */ +execute<instruction::SLA_R8>,
      /* 0x26 */ +execute<instruction::SLA_HLMEM>,    /* 0x27 */ +execute<instruction::SLA_R8>,

      /* 0x28 */ +execute<instruction::SRA_R8>,       /* 0x29 */ +execute<instruction::SRA_R8>,
      /* 0x2A */ +execute<instruction::SRA_R8>,       /* 0x2B */ +execute<instruction::SRA_R8>,
      /* 0x2C */ +execute<instruction::SRA_R8>,       /* 0x2D */ +execute<instruction::SRA_R8>,
      /* 0x2E */ +execute<instruction::SRA_HLMEM>,    /* 0x2F */ +execute<instruction::SRA_R8>,

      /* 0x30 */ +execute<instruction::SWAP_R8>,      /* 0x31 */ +execute<instruction::SWAP_R8>,
      /* 0x32 */ +execute<instruction::SWAP_R8>,      /* 0x33 */ +execute<instruction::SWAP_R8>,
      /* 0x34 */ +execute<instruction::SWAP_R8>,      /* 0x35 */ +execute<instruction::SWAP_R8>,
      /* 0x36 */ +execute<instruction::SWAP_HLMEM>,   /* 0x37 */ +execute<instruction::SWAP_R8>,

      /* 0x38 */ +execute<instruction::SRL_R8>,       /* 0x39 */ +execute<instruction::SRL_R8>,
      /* 0x3A */ +execute<instruction::SRL_R8>,       /* 0x3B */ +execute<instruction::SRL_R8>,
      /* 0x3C */ +execute<instruction::SRL_R8>,       /* 0x3D */ +execute<instruction::SRL_R8>,
      /* 0x3E */ +execute<instruction::SRL_HLMEM>,    /* 0x3F */ +execute<instruction::SRL_R8>,

      /* 0x40 */ +execute<instruction::BIT_B3_R8>,    /* 0x41 */ +execute<instruction::BIT_B3_R8>,
      /* 0x42 */ +execute<instruction::BIT_B3_R8>,    /* 0x43 */ +execute<instruction::BIT_B3_R8>,
      /* 0x44 */ +execute<instruction::BIT_B3_R8>,    /* 0x45 */ +execute<instruction::BIT_B3_R8>,
      /* 0x46 */ +execute<instruction::BIT_B3_HLMEM>, /* 0x47 */ +execute<instruction::BIT_B3_R8>,

      /* 0x48 */ +execute<instruction::BIT_B3_R8>,    /* 0x49 */ +execute<instruction::BIT_B3_R8>,
      /* 0x4A */ +execute<instruction::BIT_B3_R8>,    /* 0x4B */ +execute<instruction::BIT_B3_R8>,
      /* 0x4C */ +execute<instruction::BIT_B3_R8>,    /* 0x4D */ +execute<instruction::BIT_B3_R8>,
      /* 0x4E */ +execute<instruction::BIT_B3_HLMEM>, /* 0x4F */ +execute<instruction::BIT_B3_R8>,

      /* 0x50 */ +execute<instruction::BIT_B3_R8>,    /* 0x51 */ +execute<instruction::BIT_B3_R8>,
      /* 0x52 */ +execute<instruction::BIT_B3_R8>,    /* 0x53 */ +execute<instruction::BIT_B3_R8>,
      /* 0x54 */ +execute<instruction::BIT_B3_R8>,    /* 0x55 */ +execute<instruction::BIT_B3_R8>,
      /* 0x56 */ +execute<instruction::BIT_B3_HLMEM>, /* 0x57 */ +execute<instruction::BIT_B3_R8>,

      /* 0x58 */ +execute<instruction::BIT_B3_R8>,    /* 0x59 */ +execute<instruction::BIT_B3_R8>,
      /* 0x5A */ +execute<instruction::BIT_B3_R8>,    /* 0x5B */ +execute<instruction::BIT_B3_R8>,
      /* 0x5C */ +execute<instruction::BIT_B3_R8>,    /* 0x5D */ +execute<instruction::BIT_B3_R8>,
      /* 0x5E */ +execute<instruction::BIT_B3_HLMEM>, /* 0x5F */ +execute<instruction::BIT_B3_R8>,

      /* 0x60 */ +execute<instruction::BIT_B3_R8>,    /* 0x61 */ +execute<instruction::BIT_B3_R8>,
      /* 0x62 */ +execute<instruction::BIT_B3_R8>,    /* 0x63 */ +execute<instruction::BIT_B3_R8>,
      /* 0x64 */ +execute<instruction::BIT_B3_R8>,    /* 0x65 */ +execute<instruction::BIT_B3_R8>,
      /* 0x66 */ +execute<instruction::BIT_B3_HLMEM>, /* 0x67 */ +execute<instruction::BIT_B3_R8>,

      /* 0x68 */ +execute<instruction::BIT_B3_R8>,    /* 0x69 */ +execute<instruction::BIT_B3_R8>,
      /* 0x6A */ +execute<instruction::BIT_B3_R8>,    /* 0x6B */ +execute<instruction::BIT_B3_R8>,
      /* 0x6C */ +execute<instruction::BIT_B3_R8>,    /* 0x6D */ +execute<instruction::BIT_B3_R8>,
      /* 0x6E */ +execute<instruction::BIT_B3_HLMEM>, /* 0x6F */ +execute<instruction::BIT_B3_R8>,

      /* 0x70 */ +execute<instruction::BIT_B3_R8>,    /* 0x71 */ +execute<instruction::BIT_B3_R8>,
      /* 0x72 */ +execute<instruction::BIT_B3_R8>,    /* 0x73 */ +execute<instruction::BIT_B3_R8>,
      /* 0x74 */ +execute<instruction::BIT_B3_R8>,    /* 0x75 */ +execute<instruction::BIT_B3_R8>,
      /* 0x76 */ +execute<instruction::BIT_B3_HLMEM>, /* 0x77 */ +execute<instruction::BIT_B3_R8>,

      /* 0x78 */ +execute<instruction::BIT_B3_R8>,    /* 0x79 */ +execute<instruction::BIT_B3_R8>,
      /* 0x7A */ +execute<instruction::BIT_B3_R8>,    /* 0x7B */ +execute<instruction::BIT_B3_R8>,
      /* 0x7C */ +execute<instruction::BIT_B3_R8>,    /* 0x7D */ +execute<instruction::BIT_B3_R8>,
      /* 0x7E */ +execute<instruction::BIT_B3_HLMEM>, /* 0x7F */ +execute<instruction::BIT_B3_R8>,

      /* 0x80 */ +execute<instruction::RES_B3_R8>,    /* 0x81 */ +execute<instruction::RES_B3_R8>,
      /* 0x82 */ +execute<instruction::RES_B3_R8>,    /* 0x83 */ +execute<instruction::RES_B3_R8>,
      /* 0x84 */ +execute<instruction::RES_B3_R8>,    /* 0x85 */ +execute<instruction::RES_B3_R8>,
      /* 0x86 */ +execute<instruction::RES_B3_HLMEM>, /* 0x87 */ +execute<instruction::RES_B3_R8>,

      /* 0x88 */ +execute<instruction::RES_B3_R8>,    /* 0x89 */ +execute<instruction::RES_B3_R8>,
      /* 0x8A */ +execute<instruction::RES_B3_R8>,    /* 0x8B */ +execute<instruction::RES_B3_R8>,
      /* 0x8C */ +execute<instruction::RES_B3_R8>,    /* 0x8D */ +execute<instruction::RES_B3_R8>,
      /* 0x8E */ +execute<instruction::RES_B3_HLMEM>, /* 0x8F */ +execute<instruction::RES_B3_R8>,

      /* 0x90 */ +execute<instruction::RES_B3_R8>,    /* 0x91 */ +execute<instruction::RES_B3_R8>,
      /* 0x92 */ +execute<instruction::RES_B3_R8>,    /* 0x93 */ +execute<instruction::RES_B3_R8>,
      /* 0x94 */ +execute<instruction::RES_B3_R8>,    /* 0x95 */ +execute<instruction::RES_B3_R8>,
      /* 0x96 */ +execute<instruction::RES_B3_HLMEM>, /* 0x97 */ +execute<instruction::RES_B3_R8>,

      /* 0x98 */ +execute<instruction::RES_B3_R8>,    /* 0x99 */ +execute<instruction::RES_B3_R8>,
      /* 0x9A */ +execute<instruction::RES_B3_R8>,    /* 0x9B */ +execute<instruction::RES_B3_R8>,
      /* 0x9C */ +execute<instruction::RES_B3_R8>,    /* 0x9D */ +execute<instruction::RES_B3_R8>,
      /* 0x9E */ +execute<instruction::RES_B3_HLMEM>, /* 0x9F */ +execute<instruction::RES_B3_R8>,

      /* 0xA0 */ +execute<instruction::RES_B3_R8>,    /* 0xA1 */ +execute<instruction::RES_B3_R8>,
      /* 0xA2 */ +execute<instruction::RES_B3_R8>,    /* 0xA3 */ +execute<instruction::RES_B3_R8>,
      /* 0xA4 */ +execute<instruction::RES_B3_R8>,    /* 0xA5 */ +execute<instruction::RES_B3_R8>,
      /* 0xA6 */ +execute<instruction::RES_B3_HLMEM>, /* 0xA7 */ +execute<instruction::RES_B3_R8>,

      /* 0xA8 */ +execute<instruction::RES_B3_R8>,    /* 0xA9 */ +execute<instruction::RES_B3_R8>,
      /* 0xAA */ +execute<instruction::RES_B3_R8>,    /* 0xAB */ +execute<instruction::RES_B3_R8>,
      /* 0xAC */ +execute<instruction::RES_B3_R8>,    /* 0xAD */ +execute<instruction::RES_B3_R8>,
      /* 0xAE */ +execute<instruction::RES_B3_HLMEM>, /* 0xAF */ +execute<instruction::RES_B3_R8>,

      /* 0xB0 */ +execute<instruction::RES_B3_R8>,    /* 0xB1 */ +execute<instruction::RES_B3_R8>,
      /* 0xB2 */ +execute<instruction::RES_B3_R8>,    /* 0xB3 */ +execute<instruction::RES_B3_R8>,
      /* 0xB4 */ +execute<instruction::RES_B3_R8>,    /* 0xB5 */ +execute<instruction::RES_B3_R8>,
      /* 0xB6 */ +execute<instruction::RES_B3_HLMEM>, /* 0xB7 */ +execute<instruction::RES_B3_R8>,

      /* 0xB8 */ +execute<instruction::RES_B3_R8>,    /* 0xB9 */ +execute<instruction::RES_B3_R8>,
      /* 0xBA */ +execute<instruction::RES_B3_R8>,    /* 0xBB */ +execute<instruction::RES_B3_R8>,
      /* 0xBC */ +execute<instruction::RES_B3_R8>,    /* 0xBD */ +execute<instruction::RES_B3_R8>,
      /* 0xBE */ +execute<instruction::RES_B3_HLMEM>, /* 0xBF */ +execute<instruction::RES_B3_R8>,

      /* 0xC0 */ +execute<instruction::SET_B3_R8>,    /* 0xC1 */ +execute<instruction::SET_B3_R8>,
      /* 0xC2 */ +execute<instruction::SET_B3_R8>,    /* 0xC3 */ +execute<instruction::SET_B3_R8>,
      /* 0xC4 */ +execute<instruction::SET_B3_R8>,    /* 0xC5 */ +execute<instruction::SET_B3_R8>,
      /* 0xC6 */ +execute<instruction::SET_B3_HLMEM>, /* 0xC7 */ +execute<instruction::SET_B3_R8>,

      /* 0xC8 */ +execute<instruction::SET_B3_R8>,    /* 0xC9 */ +execute<instruction::SET_B3_R8>,
      /* 0xCA */ +execute<instruction::SET_B3_R8>,    /* 0xCB */ +execute<instruction::SET_B3_R8>,
      /* 0xCC */ +execute<instruction::SET_B3_R8>,    /* 0xCD */ +execute<instruction::SET_B3_R8>,
      /* 0xCE */ +execute<instruction::SET_B3_HLMEM>, /* 0xCF */ +execute<instruction::SET_B3_R8>,

      /* 0xD0 */ +execute<instruction::SET_B3_R8>,    /* 0xD1 */ +execute<instruction::SET_B3_R8>,
      /* 0xD2 */ +execute<instruction::SET_B3_R8>,    /* 0xD3 */ +execute<instruction::SET_B3_R8>,
      /* 0xD4 */ +execute<instruction::SET_B3_R8>,    /* 0xD5 */ +execute<instruction::SET_B3_R8>,
      /* 0xD6 */ +execute<instruction::SET_B3_HLMEM>, /* 0xD7 */ +execute<instruction::SET_B3_R8>,

      /* 0xD8 */ +execute<instruction::SET_B3_R8>,    /* 0xD9 */ +execute<instruction::SET_B3_R8>,
      /* 0xDA */ +execute<instruction::SET_B3_R8>,    /* 0xDB */ +execute<instruction::SET_B3_R8>,
      /* 0xDC */ +execute<instruction::SET_B3_R8>,    /* 0xDD */ +execute<instruction::SET_B3_R8>,
      /* 0xDE */ +execute<instruction::SET_B3_HLMEM>, /* 0xDF */ +execute<instruction::SET_B3_R8>,

      /* 0xE0 */ +execute<instruction::SET_B3_R8>,    /* 0xE1 */ +execute<instruction::SET_B3_R8>,
      /* 0xE2 */ +execute<instruction::SET_B3_R8>,    /* 0xE3 */ +execute<instruction::SET_B3_R8>,
      /* 0xE4 */ +execute<instruction::SET_B3_R8>,    /* 0xE5 */ +execute<instruction::SET_B3_R8>,
      /* 0xE6 */ +execute<instruction::SET_B3_HLMEM>, /* 0xE7 */ +execute<instruction::SET_B3_R8>,

      /* 0xE8 */ +execute<instruction::SET_B3_R8>,    /* 0xE9 */ +execute<instruction::SET_B3_R8>,
      /* 0xEA */ +execute<instruction::SET_B3_R8>,    /* 0xEB */ +execute<instruction::SET_B3_R8>,
      /* 0xEC */ +execute<instruction::SET_B3_R8>,    /* 0xED */ +execute<instruction::SET_B3_R8>,
      /* 0xEE */ +execute<instruction::SET_B3_HLMEM>, /* 0xEF */ +execute<instruction::SET_B3_R8>,

      /* 0xF0 */ +execute<instruction::SET_B3_R8>,    /* 0xF1 */ +execute<instruction::SET_B3_R8>,
      /* 0xF2 */ +execute<instruction::SET_B3_R8>,    /* 0xF3 */ +execute<instruction::SET_B3_R8>,
      /* 0xF4 */ +execute<instruction::SET_B3_R8>,    /* 0xF5 */ +execute<instruction::SET_B3_R8>,
      /* 0xF6 */ +execute<instruction::SET_B3_HLMEM>, /* 0xF7 */ +execute<instruction::SET_B3_R8>,

      /* 0xF8 */ +execute<instruction::SET_B3_R8>,    /* 0xF9 */ +execute<instruction::SET_B3_R8>,
      /* 0xFA */ +execute<instruction::SET_B3_R8>,    /* 0xFB */ +execute<instruction::SET_B3_R8>,
      /* 0xFC */ +execute<instruction::SET_B3_R8>,    /* 0xFD */ +execute<instruction::SET_B3_R8>,
      /* 0xFE */ +execute<instruction::SET_B3_HLMEM>, /* 0xFF */ +execute<instruction::SET_B3_R8>,
  }};
};

template <typename Bus>
void InstructionDecoder::execute_cb(CPU<Bus>* cpu) {
  // Move past the CB prefix
  const uint8_t cb_opcode = cpu->pc().read_opcode_byte();
  cpu->tick();
  InstructionHandler<Bus>::cb_opcode_array[cb_opcode](cpu);
}

template <typename Bus>
void InstructionDecoder::decode_and_execute(uint8_t opcode, CPU<Bus>* cpu) {
  VERBOSE_PRINT() << "Instruction:" << StringUtils::binary(opcode) << " Hex: " << StringUtils::hex(opcode)
                  << " Oct: " << StringUtils::oct(opcode) << std::endl;

  if (opcode == 0xCB) {
    execute_cb(cpu);
  } else {
    InstructionHandler<Bus>::opcode_array[opcode](cpu);
  }
}