----------------------------------------------------------------------------------
-- Company: Department of Electrical and Computer Engineering, University of Alberta
-- Engineer: Shyama Gandhi and Bruce Cockburn
--
-- Create Date: 10/29/2020 07:18:24 PM
-- Design Name: CONTROLLER FOR THE CPU
-- Module Name: cpu - behavioral(controller)
-- Project Name:
-- Target Devices:
-- Tool Versions:
-- Description: CPU_LAB 3 - ECE 410 (2020)
--
-- Dependencies:
--
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--*********************************************************************************
-- The controller implements the states for each instructions and asserts appropriate control signals for the datapath during every state.
-- For detailed information on the opcodes and instructions to be executed, refer the lab manual.
-----------------------------
LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL; -- needed for CONV_INTEGER()

ENTITY controller IS PORT (
  clk_ctrl : IN STD_LOGIC;
  rst_ctrl : IN STD_LOGIC;
  enter : IN STD_LOGIC;
  muxsel_ctrl : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
  imm_ctrl : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
  accwr_ctrl : OUT STD_LOGIC;
  rfaddr_ctrl : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
  rfwr_ctrl : OUT STD_LOGIC;
  alusel_ctrl : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
  outen_ctrl : OUT STD_LOGIC;
  zero_ctrl : IN STD_LOGIC;
  positive_ctrl : IN STD_LOGIC;
  PC_out : OUT STD_LOGIC_VECTOR(4 DOWNTO 0);
  OP_out : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
  done : OUT STD_LOGIC);
END controller;

ARCHITECTURE Behavior OF controller IS

  TYPE state_type IS (Fetch, Decode, LDA_execute, STA_execute, LDI_execute, ADD_execute, SUB_execute, SHFL_execute, SHFR_execute,
    input_A, output_A, Halt_cpu, JZ_execute, flag_state, ADD_SUB_SL_SR_next);

  SIGNAL state : state_type;

  -- Instructions and their opcodes (pre-decided)
  CONSTANT LDA : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0001";
  CONSTANT STA : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0010";
  CONSTANT LDI : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0011";

  CONSTANT ADD : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0100";
  CONSTANT SUB : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0101";

  CONSTANT SHFL : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0110";
  CONSTANT SHFR : STD_LOGIC_VECTOR(3 DOWNTO 0) := "0111";

  CONSTANT INA : STD_LOGIC_VECTOR(3 DOWNTO 0) := "1000";
  CONSTANT OUTA : STD_LOGIC_VECTOR(3 DOWNTO 0) := "1001";
  CONSTANT HALT : STD_LOGIC_VECTOR(3 DOWNTO 0) := "1010";

  CONSTANT JZ : STD_LOGIC_VECTOR(3 DOWNTO 0) := "1100";

  TYPE PM_BLOCK IS ARRAY(0 TO 31) OF STD_LOGIC_VECTOR(7 DOWNTO 0); -- program memory that will store the instructions sequentially from part 1 and part 2 test program

  FUNCTION part_1 RETURN PM_BLOCK IS
    VARIABLE PM : PM_BLOCK;
  BEGIN
    PM(0) := "10000000";
    PM(1) := "10000001";
    PM(2) := "11000000";
    PM(3) := "00000100";
    PM(4) := "01100000";
    PM(5) := "10010000";
    PM(6) := "00110000";
    PM(7) := "00001010";
    PM(8) := "01110000";
    PM(9) := "10010000";
    PM(10) := "10100000";
    RETURN PM;
  END FUNCTION part_1;

  FUNCTION part_2 RETURN PM_BLOCK IS
    VARIABLE PM : PM_BLOCK;
  BEGIN
    PM(0) := "10000000";
    PM(1) := "10000001";
    PM(2) := "00100000";
    PM(3) := "00110000";
    PM(4) := "00001111";
    PM(5) := "00100000";
    PM(6) := "10010000";
    PM(10) := "10100000";
    RETURN PM;
  END FUNCTION part_2;

BEGIN
  PROCESS (clk_ctrl) -- complete the sensitivity list

    -- "PM" is the program memory that holds the instructions to be executed by the CPU 
    VARIABLE PM : PM_BLOCK;
    -- Instruction once fetched will be stored in the IR register                   
    VARIABLE IR : STD_LOGIC_VECTOR(7 DOWNTO 0);
    -- To decode the 4 MSBs from the PC content
    VARIABLE OPCODE : STD_LOGIC_VECTOR(3 DOWNTO 0);
    -- PC pointing to the program memory
    VARIABLE PC : INTEGER RANGE 0 TO 31;
    -- Zero flag and positive flag
    VARIABLE zero_flag, positive_flag : STD_LOGIC;

  BEGIN
    IF (rst_ctrl = '1') THEN -- RESET initializes all the control signals to 0.
      PC := 0;
      muxsel_ctrl <= "00";
      imm_ctrl <= (OTHERS => '0');
      accwr_ctrl <= '0';
      rfaddr_ctrl <= "000";
      rfwr_ctrl <= '0';
      alusel_ctrl <= "000";
      outen_ctrl <= '0';
      done <= '0';
      state <= Fetch;

      -- *************** assembly code for PART1/PART2 goes here
      --                PM(0) := "XXXXXXXX"; -- for example this is how the instructions will be stored in the program memory
      -- **************
      PM := part_1;

    ELSIF (clk_ctrl'event AND clk_ctrl = '1') THEN
      CASE state IS
        WHEN Fetch => -- fetch instruction
          IF (enter = '1') THEN
            PC_out <= conv_std_logic_vector(PC, 5);
            IR := PM(PC);
            -- ****************************************
            -- write one line of code to get the opcode from the IR
            OPCODE := IR(7 DOWNTO 4);
            -------------------------------------------
            OP_out <= OPCODE;
            PC := PC + 1;
            muxsel_ctrl <= "00";
            imm_ctrl <= (OTHERS => '0');
            accwr_ctrl <= '0';
            rfaddr_ctrl <= "000";
            rfwr_ctrl <= '0';
            alusel_ctrl <= "000";
            outen_ctrl <= '0';
            done <= '0';
            state <= Decode;
          ELSIF (enter = '0') THEN
            state <= Fetch;
          END IF;

        WHEN Decode => -- decode instruction
          CASE OPCODE IS
            WHEN LDA => state <= LDA_execute;
            WHEN STA => state <= STA_execute;
            WHEN LDI => state <= LDI_execute;
            WHEN ADD => state <= ADD_execute;
            WHEN SUB => state <= SUB_execute;
            WHEN SHFL => state <= SHFL_execute;
            WHEN SHFR => state <= SHFR_execute;
            WHEN INA => state <= input_A;
            WHEN OUTA => state <= output_A;
            WHEN HALT => state <= Halt_cpu;
            WHEN JZ => state <= JZ_execute;
            WHEN OTHERS => state <= Halt_cpu;

          END CASE;

          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '0';
          rfaddr_ctrl <= "000";
          rfwr_ctrl <= '0';
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          done <= '0';

        WHEN flag_state => -- set zero and positive flags and then goto next instruction
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '0';
          rfaddr_ctrl <= "000";
          rfwr_ctrl <= '0';
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          done <= '0';
          state <= Fetch;
          zero_flag := zero_ctrl;
          positive_flag := positive_ctrl;

        WHEN ADD_SUB_SL_SR_next => -- next state TO add, sub,shfl, shfr
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '1';
          rfaddr_ctrl <= "000";
          rfwr_ctrl <= '0';
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          state <= Fetch;

        WHEN LDA_execute => -- LDA 
          -- *********************************
          -- write the entire state for LDA_execute
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '1'; -- write to accumulator
          rfaddr_ctrl <= IR(2 DOWNTO 0);
          rfwr_ctrl <= '0'; -- read from register file
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          done <= '0';
          state <= Fetch;

        WHEN STA_execute => -- STA 
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '0';
          rfaddr_ctrl <= IR(2 DOWNTO 0);
          rfwr_ctrl <= '1';
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          done <= '0';
          state <= Fetch;

        WHEN LDI_execute => -- LDI 
          -- *********************************
          -- write the entire state for LDI_execute
          muxsel_ctrl <= "11";
          PC := PC + 1;
          imm_ctrl <= PM(PC);
          accwr_ctrl <= '1'; -- write to accumulator
          rfaddr_ctrl <= IR(2 DOWNTO 0);
          rfwr_ctrl <= '0';
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          done <= '0';
          state <= Fetch;
        WHEN JZ_execute => -- JZ
          -- *********************************
          -- write the entire state for JZ_execute
          muxsel_ctrl <= "11";
          imm_ctrl <= PM(PC + 1);
          accwr_ctrl <= '1';
          rfaddr_ctrl <= "000";
          rfwr_ctrl <= '0';
          alusel_ctrl <= "100";
          outen_ctrl <= '0';
          done <= '0';
          state <= Fetch;
        WHEN ADD_execute => -- ADD 
          -- *********************************
          -- write the entire state for ADD_execute
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '1';
          rfaddr_ctrl <= IR(2 DOWNTO 0);
          rfwr_ctrl <= '0';
          alusel_ctrl <= "100";
          outen_ctrl <= '0';
          done <= '0';
          state <= ADD_SUB_SL_SR_next;
        WHEN SUB_execute => -- SUB 
          -- *********************************
          -- write the entire state for SUB_execute
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '1';
          rfaddr_ctrl <= IR(2 DOWNTO 0);
          rfwr_ctrl <= '0';
          alusel_ctrl <= "101";
          outen_ctrl <= '0';
          done <= '0';
          state <= ADD_SUB_SL_SR_next;
        WHEN SHFL_execute => -- SHFL
          -- *********************************
          -- write the entire state for SHFL_execute
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '1';
          rfaddr_ctrl <= IR(2 DOWNTO 0);
          rfwr_ctrl <= '0';
          alusel_ctrl <= "101";
          outen_ctrl <= '0';
          done <= '0';
          state <= ADD_SUB_SL_SR_next;
        WHEN SHFR_execute => -- SHFR 
          -- *********************************
          -- write the entire state for SHFR_execute
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '1';
          rfaddr_ctrl <= IR(2 DOWNTO 0);
          rfwr_ctrl <= '0';
          alusel_ctrl <= "110";
          outen_ctrl <= '0';
          done <= '0';
          state <= ADD_SUB_SL_SR_next;
        WHEN input_A => -- INA
          muxsel_ctrl <= "10";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '1';
          rfaddr_ctrl <= "000";
          rfwr_ctrl <= '0';
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          done <= '0';
          state <= flag_state;

        WHEN output_A => -- OUTA
          -- *********************************
          -- write the entire state for output_A
        WHEN Halt_cpu => -- HALT
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '0';
          rfaddr_ctrl <= "000";
          rfwr_ctrl <= '0';
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          done <= '0';
          state <= Halt_cpu;

        WHEN OTHERS =>
          muxsel_ctrl <= "00";
          imm_ctrl <= (OTHERS => '0');
          accwr_ctrl <= '0';
          rfaddr_ctrl <= "000";
          rfwr_ctrl <= '0';
          alusel_ctrl <= "000";
          outen_ctrl <= '0';
          done <= '0';
          state <= Halt_cpu;
      END CASE;
    END IF;

  END PROCESS;
END Behavior;