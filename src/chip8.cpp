#include "chip8.h"

#include <cstdio>
#include <cstdlib>
//#include <cstdint>
//#include <vector>

#define TRUE 1
#define FALSE 0

std::vector<uint8_t> chip8_fontset = 
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void Chip8::initialize() {
  PC = (0x200);
  opcode = 0;
  I = 0;
  SP = 0;
  stack.resize(16, 0);
  V.resize(16, 0);
  memory.resize(4096, 0);
  FB.resize(32, std::vector<uint8_t>(64, 0));
  key_state.resize(16, 0);

  for(int i=0; i<80; ++i) {
    memory[0x50 + i] = chip8_fontset[i];
  }

  for(int i=0; i<32; ++i) {
    for(int j=0; j<64; ++j) {
      FB[i][j] = 0;
    }
  }

  ST = 0;
  DT = 0;

  stop = 0;
}

int Chip8::loadGame(const char* filename) {

  //printf("Loading file: %s\n", filename);
  FILE* file = fopen(filename, "rb");
  if(file == nullptr) {
    printf("Failed to open file");
    return 0;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);
  //printf("Filesize: %d\n", file_size);

  unsigned char* buffer = (unsigned char*)malloc( sizeof(unsigned char) * file_size);
  if(buffer == nullptr) {
    printf("Failed to allocate memory");
    fclose(file);
    return 0;
  }

  size_t result = fread(buffer, 1, file_size, file);
  if(result != file_size) {
    printf("Failed to read file");
    free(buffer);
    fclose(file);
    return 0;
  }
  //printf("Res: %d\n", result);

  if( (4096-512) > file_size) {
    for(int i = 0; i < file_size; ++i) {
      memory[0x200 + i] = buffer[i];
    }
  }

  //printf("Buffer: %d\n", buffer[1]);

  fclose(file);
  free(buffer);
  return 1;
}

void Chip8::emulateCycle() {

  if(!stop) {

    printf("PC at cycle start: %X\n", PC);
    fetch();
    printf("Opcode after fetch: %X\n", opcode);
    for(int i=0; i<16; ++i) {
      printf("V%X: %X, ", i, V[i]);
      if(V[i] < 0x10) {
        printf(" ");
      }
      if(i == 7) {
        printf("\n");
      }
    }
    printf("\nI: %X\n", I);
    decode();
    dt_count();
    st_count();

  }
}

void Chip8::fetch() {
  uint8_t upper_bits = memory[PC];
  uint8_t lower_bits = memory[PC+1];

  opcode = ((upper_bits << 8) | lower_bits);
}

void Chip8::decode() {
  printf("Decoding: %X\n", (0xF000 & opcode)>>12 );
  switch( (0xF000 & opcode) >> 12) {
    case 0x0:
      switch(0x000F & opcode) {
        case 0x0:
          printf("Calling cls\n");
          cls();
          break;

        case 0xE:
          puts("Calling ret");
          ret();
          break;

        default:
          puts("Opcode not recognised");
      }
      break;

    case 0x1:
      puts("Calling jump");
      jump();
      break;

    case 0x2:
      puts("Calling call_sub");
      call_sub();
      break;

    case 0x3:
      puts("Calling se_value");
      se_value();
      break;

    case 0x4:
      puts("Calling sne_value");
      sne_value();
      break;

    case 0x5:
      puts("Calling se_registers");
      se_registers();
      break;

    case 0x6:
      puts("Calling ld_value");
      ld_value();
      break;

    case 0x7:
      puts("Calling add_value");
      add_value();
      break;

    case 0x8:
      switch(0x000F & opcode) {
        case 0x0:
          puts("Calling ld_registers");
          ld_registers();
          break;

        case 0x1:
          puts("Calling or_registers");
          or_registers();
          break;

        case 0x2:
          puts("Calling and_registers");
          and_registers();
          break;

        case 0x3:
          puts("Calling xor_registers");
          xor_registers();
          break;

        case 0x4:
          puts("Calling add_registers");
          add_registers();
          break;

        case 0x5:
          puts("Calling sub_registers");
          sub_registers();
          break;

        case 0x6:
          puts("Calling shr_registers");
          shr_registers();
          break;

        case 0x7:
          puts("Calling subn_registers");
          subn_registers();
          break;

        case 0xE:
          puts("Calling shl_registers");
          shl_registers();
          break;

        default:
          puts("Opcode not recognised");
      }
      break;

    case 0x9:
      puts("Calling sne_registers");
      sne_registers();
      break;

    case 0xA:
      puts("Calling ldi");
      ld_I();
      break;

    case 0xB:
      puts("Calling jump_v0");
      jump_v0();
      break;

    case 0xD:
      puts("Calling draw");
      draw(); // Dxyn
      break;

    case 0xE:
      switch(0x00FF & opcode) {
        case 0x9E:
          puts("Calling skp_vx");
          skp_vx();
          break;

        case 0xA1:
          puts("Calling sknp_vx");
          sknp_vx();
          break;

        default:
          puts("Opcode not recognised");
      }
      break;

    case 0xF:
      switch(0x00FF & opcode) {
        case 0x07:
          puts("Calling ld_vx_dt");
          ld_vx_dt();
          break;

        case 0x0A:
          puts("Calling ld_vx_k");
          ld_vx_k();
          break;

        case 0x15:
          puts("Calling ld_dt_vx");
          ld_dt_vx();
          break;

        case 0x1E:
          puts("Calling add_I_vx");
          add_I_vx();
          break;

        case 0x33:
          puts("Calling ld_b_vx");
          ld_b_vx();
          break;

        case 0x55:
          puts("Calling ld_I_vx");
          ld_I_vx();
          break;

        case 0x65:
          puts("Calling ld_vx_I");
          ld_vx_I();
          break;

        default:
          puts("Opcode not recognised");
      }
      break;

    default:
      puts("Opcode not recognised");
  }
}

void Chip8::cls() {
  for(int i=0; i<32; ++i) {
    for(int j=0; j<64; ++j) {
      FB[i][j] = 0;
    }
  }
  draw_flag = TRUE;
  PC += 2;
  printf("PC after cls: %X\n", PC);
}

void Chip8::ret() {
  PC = stack[SP-1];
  SP -= 1;
  PC += 2;
  printf("PC after ret: %X\n", PC);
}

void Chip8::jump() {
  uint16_t nnn = (0x0FFF & opcode);
  PC = nnn;
  printf("PC after jump: %X\n", PC);
}

void Chip8::call_sub() {
  stack[SP] = PC;
  uint16_t nnn = (0x0FFF & opcode);
  PC = nnn;
  SP++;
  printf("PC after call_sub: %X\n", PC);
}

void Chip8::se_value() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t kk = (0x00FF & opcode);
  if(V[x] == kk) {
    PC += 2;
  }
  PC += 2;
  printf("PC after se_value: %X\n", PC);
}

void Chip8::sne_value() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t kk = (0x00FF & opcode);
  if(V[x] != kk) {
    PC += 2;
  }
  PC += 2;
  printf("PC after sne_value: %X\n", PC);
}

void Chip8::se_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  if(V[x] == V[y]) {
    PC += 2;
  }
  PC += 2;
  printf("PC after se_registers: %X\n", PC);
}

void Chip8::ld_value() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t kk = (0x00FF & opcode);
  V[x] = kk;
  PC += 2;
  printf("PC after ld_value: %X\n", PC);
}

void Chip8::add_value() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t kk = (0x00FF & opcode);
  V[x] += kk;
  PC += 2;
  printf("PC after add_value: %X\n", PC);
}

void Chip8::ld_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  V[x] = V[y];
  PC += 2;
  printf("PC after ld_registers: %X\n", PC);
}

void Chip8::or_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  V[x] = ( V[x] | V[y] );
  PC += 2;
  printf("PC after or_registers: %X\n", PC);
}

void Chip8::and_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  V[x] = ( V[x] & V[y] );
  PC += 2;
  printf("PC after and_registers: %X\n", PC);
}

void Chip8::xor_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  V[x] = ( V[x] ^ V[y] );
  PC += 2;
  printf("PC after xor_registers: %X\n", PC);
}

void Chip8::add_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  uint8_t carry;
  if( (V[x] + V[y]) > 255 ) {
    carry = 1;
  } else {
    carry = 0;
  }
  V[x] += V[y];
  V[0xF] = carry;
  PC += 2;
  printf("PC after add_registers: %X\n", PC);
}

void Chip8::sub_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  uint8_t notBorrow;
  if( (V[x] >= V[y]) ) {
    notBorrow = 1;
  } else {
    notBorrow = 0;
  }
  V[x] = V[x] - V[y];
  V[0xF] = notBorrow;
  PC += 2;
  printf("PC after sub_registers: %X\n", PC);
}

void Chip8::shr_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  uint8_t shiftedOut;
  if( (V[y] & 0x1) == 1) {
    shiftedOut = 1;
  } else {
    shiftedOut = 0;
  }
  V[x] = ( V[y] >> 1);
  V[0xF] = shiftedOut;
  PC += 2;
  printf("PC after shr_registers: %X\n", PC);
}

void Chip8::subn_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  uint8_t notBorrow;
  if( (V[y] >= V[x]) ) {
    notBorrow = 1;
  } else {
    notBorrow = 0;
  }
  V[x] = V[y] - V[x];
  V[0xF] = notBorrow;
  PC += 2;
  printf("PC after subn_registers: %X\n", PC);
}

void Chip8::shl_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  uint8_t shiftedOut;
  if( (V[y] & 0x80) >> 7 == 1) {
    shiftedOut = 1;
  } else {
    shiftedOut = 0;
  }
  V[x] = ( V[y] << 1);
  V[0xF] = shiftedOut;
  PC += 2;
  printf("PC after shl_registers: %X\n", PC);
}

void Chip8::sne_registers() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  uint8_t y = (0x00F0 & opcode) >> 4;
  if(V[x] != V[y]) {
    PC += 2;
  }
  PC += 2;
  printf("PC after sne_registers: %X\n", PC);
}

void Chip8::ld_I() {
  uint16_t nnn = (0x0FFF & opcode);
  I = nnn;
  PC += 2;
  printf("PC after ldi: %X\n", PC);
}

void Chip8::jump_v0() {
  uint16_t nnn = (0x0FFF & opcode);
  PC = nnn + V[0];
  printf("PC after jump_v0: %X\n", PC);
}

void Chip8::draw() { // Dxyn
  uint8_t x = V[ (0x0F00 & opcode) >> 8];
  uint8_t y = V[ (0x00F0 & opcode) >> 4];
  uint8_t height = (0x000F & opcode);
  uint8_t pixel;
  uint8_t collision = 0;

  for(int yline = 0; yline < height; ++yline) {
    pixel = memory[I + yline];
    for(int xline = 0; xline< 8; ++xline) {
      if( (pixel & (0x80 >> xline)) != 0) {
        if( FB[y + yline][x + xline] == 1) {
          collision = 1;
        }
        FB[y + yline][x + xline] ^= 1;
      }
    }
  }

  V[0xF] = collision;
  draw_flag = TRUE;
  PC += 2;
  printf("PC after draw: %X\n", PC);
}

int Chip8::get_draw_flag() {
  return draw_flag;
}

void Chip8::reset_draw_flag() {
  draw_flag = FALSE;
}

std::vector<std::vector<uint8_t>> Chip8::get_fb() {
  return FB;
}

void Chip8::skp_vx() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  if(key_state[ V[x] ] == 1) {
    PC += 2;
  }
  PC += 2;
  printf("PC after skp_vx: %X\n", PC);
}

void Chip8::sknp_vx() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  if(key_state[ V[x] ] == 0) {
    PC += 2;
  }
  PC += 2;
  printf("PC after sknp_vx: %X\n", PC);
}

void Chip8::ld_vx_dt() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  V[x] = DT;
  PC += 2;
  printf("PC after ld_vx_dt: %X\n", PC);
}

void Chip8::ld_vx_k() {
  //uint8_t x = (0x0F00 & opcode) >> 8;
  stop = 1;
  //PC += 2;
  printf("PC after ld_vx_k: %X\n", PC);
}

void Chip8::ld_dt_vx() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  DT = V[x];
  PC += 2;
  printf("PC after ld_dt_vx: %X\n", PC);
}

void Chip8::add_I_vx() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  I += V[x];
  PC += 2;
  printf("PC after add_I_vx: %X\n", PC);
}

void Chip8::ld_b_vx() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  memory[I+0] = V[x] / 100;
  memory[I+1] = ( V[x] % 100 ) / 10;
  memory[I+2] = V[x] % 10;
  PC += 2;
  printf("PC after ld_b_vx: %X\n", PC);
}

void Chip8::ld_I_vx() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  for(int i=0; i<=x; ++i) {
    memory[I+i] = V[i];
  }
  PC += 2;
  printf("PC after ld_I_vx: %X\n", PC);
}

void Chip8::ld_vx_I() {
  uint8_t x = (0x0F00 & opcode) >> 8;
  for(int i=0; i<=x; ++i) {
    V[i] = memory[I+i];
  }
  PC += 2;
  printf("PC after ld_vx_I: %X\n", PC);
}

void Chip8::dt_count() {
  if(DT > 0) {
    DT--;
  }
}

void Chip8::st_count() {
  if(ST > 0) {
    ST--;
  }
}

void Chip8::key_down(uint8_t key) {
  key_state[key] = 1;
  if(stop == 1) { // Fx0A in execution
    uint8_t x = (0x0F00 & opcode) >> 8;
    V[x] = key;
    PC += 2;
  }
}

void Chip8::key_up(uint8_t key) {
  key_state[key] = 0;
}

// Debug functions
std::vector<uint8_t> Chip8::get_memory() {
  return memory;
}
uint16_t Chip8::get_pc() {
  return PC;
}
uint8_t Chip8::get_key() {
  return key_state[1];
}
