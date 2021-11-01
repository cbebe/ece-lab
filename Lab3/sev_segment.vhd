----------------------------------------------------------------------------------
-- Company: Department of Electrical and Computer Engineering, University of Alberta
-- Engineer: Shyama Gandhi and Bruce Cockburn
--
-- Create Date: 10/29/2020 07:18:24 PM
-- Design Name: CONTROLLER FOR THE CPU
-- Module Name: 
-- Project Name:
-- Target Devices:
-- Tool Versions:
-- Description: CPU OF LAB 3 - ECE 410 (2020)
--
-- Dependencies:
--
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--*********************************************************************************
-- This the seven segment that will display the current Program counter on any one of the two display(seven-segment)
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


entity sev_segment is
    Port ( 
		 --output of PC from cpu
		 DispVal : in  STD_LOGIC_VECTOR (4downto 0);
	         anode: out std_logic;
	         --controls which digit to display
             	 segOut : out  STD_LOGIC_VECTOR (6 downto 0)); 
end sev_segment;

architecture Behavioral of sev_segment is
begin

	anode<='1';
		
	with DispVal select
		segOut <=     "0111111" when "00000", --0
			      "0000110" when "00001", --1
			      "1011011" when "00010", --2
			      "1001111" when "00011", --3
			      "1100110" when "00100", --4
			      "1101101" when "00101", --5
			      "1111101" when "00110", --6
			      "0000111" when "00111", --7
			      "1111111" when "01000", --8
			      "1101111" when "01001", --9

				-- ***************************************
				-- write the remaining lines to display from A to F, when "others" is provided to you...

			      "1000000" when others;
	
end Behavioral;

