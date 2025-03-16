#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <vector>

class Chip8 {
private:
  std::vector<uint8_t> memory;
  // RAM memory
  // 4096 bytes

  std::vector<uint8_t> V;
  // General purpose registers
  // V0 to VF

  std::vector<uint16_t> stack;
  // PC can save values here

  uint8_t ST; // Sound timer
  uint8_t DT; // Delay timer
  uint8_t SP; // Stack pointer
  uint16_t PC; // Program counter
  uint16_t I; // Register I
  // Special registers

  std::vector<std::vector<uint8_t>> FB;

  std::vector<uint8_t> key_state;

  uint16_t opcode;
  int draw_flag;
  int stop; // Fx0A stops all execution waiting for a key

public:
  void initialize();
  int loadGame(const char*);
  void emulateCycle();
  void fetch();
  void decode ();
  void cls(); //00E0
  void ret(); // 00EE
  void jump(); // 1nnn
  void call_sub(); // 2nnn
  void se_value(); // 3xkk
  void sne_value(); // 4xkk
  void se_registers(); // 5xy0
  void ld_value(); // 6xkk
  void add_value(); // 7xkk
  void ld_registers(); // 8xy0
  void or_registers(); // 8xy1
  void and_registers(); // 8xy2
  void xor_registers(); // 8xy3
  void add_registers(); // 8xy4
  void sub_registers(); // 8xy5
  void shr_registers(); // 8xy6
  void subn_registers(); // 8xy7
  void shl_registers(); // 8xyE
  void sne_registers(); // 9xy0
  void ld_I(); // Annn
  void jump_v0(); // Bnnn
  // C function needs a random number generator, did it use the Cosmac rca 1802 rng?
  // Some guides says it is alright to use the language's built-in rng, in my case, c++
  void draw(); // D

  int get_draw_flag();
  void reset_draw_flag();
  std::vector<std::vector<uint8_t>> get_fb();

  void skp_vx(); // Ex9E
  void sknp_vx(); // ExA1

  void ld_vx_dt(); // Fx07
  void ld_vx_k(); // Fx0A
  void ld_dt_vx(); // Fx15

  void add_I_vx(); // Fx1E

  void ld_b_vx(); // Fx33
  void ld_I_vx(); // Fx55
  void ld_vx_I(); // Fx65

  void dt_count();
  void st_count();

  void key_down(uint8_t);
  void key_up(uint8_t);

  // Debug functions
  std::vector<uint8_t> get_memory();
  uint16_t get_pc();
  uint8_t get_key();
};

#endif // CHIP8_H
