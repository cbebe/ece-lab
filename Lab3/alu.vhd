----------------------------------------------------------------------------------
-- Company: Department of Electrical and Computer Engineering, University of Alberta
-- Engineer: Shyama Gandhi and Bruce Cockburn
--
-- Create Date: 10/29/2020 07:18:24 PM
-- Design Name:
-- Module Name: cpu - structural(datapath)
-- Project Name:
-- Target Devices:
-- Tool Versions:
-- Description: CPU LAB 3 - ECE 410 (2020)
--
-- Dependencies:
--
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--*********************************************************************************
-- Total eights operations can be performed using 3 select lines of this ALU.
-- The select line codes have been given to you in the lab manual.
-- In future, this alu is scalable to say, 16 operations using 4 select lines.
-----------------------------

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
-- The following package is needed so that the STD_LOGIC_VECTOR signals
-- A and B can be used in unsigned arithmetic operations.
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY alu IS PORT (
  clk_alu : IN STD_LOGIC;
  sel_alu : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
  bit_alu : IN STD_LOGIC_VECTOR(1 DOWNTO 0); -- number of bits to shift left/right
  inA_alu : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
  inB_alu : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
  OUT_alu : OUT STD_LOGIC_VECTOR (7 DOWNTO 0) := "00000000");
END alu;

ARCHITECTURE Behavior OF alu IS
BEGIN
  PROCESS (clk_alu) -- complete the sensitivity list here!

  BEGIN
    IF clk_alu'event AND clk_alu = '1' THEN
      CASE sel_alu IS
        WHEN "000" =>
          OUT_alu <= inA_alu;
        WHEN "001" =>
          OUT_alu <= inA_alu AND inB_alu;
        WHEN "010" =>
          --                                shift left with 0
          -- ***************************************
          -- write one line of code here to perform shift left
          OUT_alu <= STD_LOGIC_VECTOR(shift_left(unsigned(inA_alu),
            to_integer(unsigned(bit_alu))));
        WHEN "011" =>
          --                                shift right with 0
          -- ***************************************
          -- write one line of code here to perform shift right
          OUT_alu <= STD_LOGIC_VECTOR(shift_right(unsigned(inA_alu),
            to_integer(unsigned(bit_alu))));
        WHEN "100" =>
          OUT_alu <= inA_alu + inB_alu;
        WHEN "101" =>
          OUT_alu <= inA_alu - inB_alu;
        WHEN "110" =>
          OUT_alu <= inA_alu + 1;
        WHEN OTHERS =>
          OUT_alu <= inA_alu - 1;
      END CASE;
    END IF;
  END PROCESS;
END Behavior;