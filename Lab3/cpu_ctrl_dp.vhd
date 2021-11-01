----------------------------------------------------------------------------------
-- Company: Department of Electrical and Computer Engineering, University of Alberta
-- Engineer: Shyama Gandhi and Bruce Cockburn
--
-- Create Date: 10/29/2020 07:18:24 PM
-- Design Name: CONTROLLER AND DATAPATH FOR THE CPU
-- Module Name: cpu - structural
-- Project Name:
-- Target Devices:
-- Tool Versions:
-- Description: CPU LAB 3 - ECE 410 (2020)
--
-- Dependencies:
--
-- Revision:
-- Revision 0.01 - File Created
-- *******************************************************************************
-- Additional Comments:
-- This is the module that integrates the datapath and the controller
----------------------------------------------------------------------------------

LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.std_logic_arith.ALL;

ENTITY cpu_ctrl_dp IS PORT (
  clk_cpu : IN STD_LOGIC;
  rst_cpu : IN STD_LOGIC;
  entered_ip : IN STD_LOGIC;
  input_cpu : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
  output_cpu : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
  PC_output : OUT STD_LOGIC_VECTOR(4 DOWNTO 0);
  OPCODE_ouput : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
  done_cpu : OUT STD_LOGIC);
END cpu_ctrl_dp;

ARCHITECTURE structure OF cpu_ctrl_dp IS

  COMPONENT controller PORT (
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
  END COMPONENT;

  COMPONENT datapath PORT (
    clk_dp : IN STD_LOGIC;
    rst_dp : IN STD_LOGIC;
    muxsel_dp : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
    imm_dp : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    input_dp : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    accwr_dp : IN STD_LOGIC;
    rfaddr_dp : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    rfwr_dp : IN STD_LOGIC;
    alusel_dp : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    outen_dp : IN STD_LOGIC;
    zero_dp : OUT STD_LOGIC;
    positive_dp : OUT STD_LOGIC;
    output_dp : OUT STD_LOGIC_VECTOR(7 DOWNTO 0));
  END COMPONENT;

  SIGNAL C_immediate : STD_LOGIC_VECTOR(7 DOWNTO 0);
  SIGNAL C_accwr, C_rfwr, C_outen, C_zero, C_positive : STD_LOGIC;
  SIGNAL C_muxsel, C_alubit : STD_LOGIC_VECTOR(1 DOWNTO 0);
  SIGNAL C_rfaddr, C_alusel : STD_LOGIC_VECTOR(2 DOWNTO 0);
BEGIN
  U0 : controller PORT MAP(
    clk_ctrl => clk_cpu,
    rst_ctrl => rst_cpu,
    enter => entered_ip,
    muxsel_ctrl => C_muxsel,
    imm_ctrl => C_immediate,
    accwr_ctrl => C_accwr,

    -- *****************************
    -- map the remaining signals here
    rfaddr_ctrl => C_rfaddr,
    rfwr_ctrl => C_rfwr,
    alusel_ctrl => C_alusel,
    alubit_ctrl => C_alubit,
    outen_ctrl => C_outen,
    zero_ctrl => C_zero,
    positive_ctrl => C_positive,
    PC_out => PC_output,
    OP_out => OPCODE_ouput,
    done => done_cpu);

  U1 : datapath PORT MAP(
    clk_dp => clk_cpu,
    rst_dp => rst_cpu,
    muxsel_dp => C_muxsel,
    imm_dp => C_immediate,
    input_dp => input_cpu,
    accwr_dp => C_accwr,
    rfaddr_dp => C_rfaddr,
    rfwr_dp => C_rfwr,
    alusel_dp => C_alusel,
    alubit_dp => C_alubit,
    outen_dp => C_outen,
    -- *****************************
    -- map the remaining signals here
    zero_dp => C_zero,
    positive_dp => C_positive,
    output_dp => output_cpu);
END structure;