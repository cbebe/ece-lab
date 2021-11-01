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
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL; -- needed for CONV_INTEGER()

ENTITY controller IS PORT (
            clk_ctrl        : IN std_logic;
            rst_ctrl        : IN std_logic;
            enter           : IN std_logic;
            muxsel_ctrl     : OUT std_logic_vector(1 DOWNTO 0);
            imm_ctrl        : OUT std_logic_vector(7 DOWNTO 0);
            accwr_ctrl      : OUT std_logic;          
            rfaddr_ctrl     : OUT std_logic_vector(2 DOWNTO 0);
            rfwr_ctrl       : OUT std_logic;
            alusel_ctrl     : OUT std_logic_vector(2 DOWNTO 0);
            outen_ctrl      : OUT std_logic;
            zero_ctrl       : IN std_logic;
            positive_ctrl   : IN std_logic;
            PC_out          : out std_logic_vector(4 downto 0);
            OP_out          : out std_logic_vector(3 downto 0);
            done            : out std_logic);
END controller;

architecture Behavior of controller is

TYPE state_type IS (Fetch,Decode,LDA_execute,STA_execute,LDI_execute, ADD_execute, SUB_execute, SHFL_execute, SHFR_execute,
                    input_A, output_A, Halt_cpu, JZ_execute, flag_state, ADD_SUB_SL_SR_next);

SIGNAL state: state_type;

-- Instructions and their opcodes (pre-decided)
    CONSTANT LDA : std_logic_vector(3 DOWNTO 0) := "0001";  
    CONSTANT STA : std_logic_vector(3 DOWNTO 0) := "0010";  
    CONSTANT LDI : std_logic_vector(3 DOWNTO 0) := "0011";  
    
    CONSTANT ADD : std_logic_vector(3 DOWNTO 0) := "0100"; 
    CONSTANT SUB : std_logic_vector(3 DOWNTO 0) := "0101"; 
    
    CONSTANT SHFL : std_logic_vector(3 DOWNTO 0) := "0110";  
    CONSTANT SHFR : std_logic_vector(3 DOWNTO 0) := "0111";  
    
    CONSTANT INA  : std_logic_vector(3 DOWNTO 0) := "1000";   
    CONSTANT OUTA : std_logic_vector(3 DOWNTO 0) := "1001";   
    CONSTANT HALT : std_logic_vector(3 DOWNTO 0) := "1010";   
    
    CONSTANT JZ   : std_logic_vector(3 DOWNTO 0) := "1100";
    
    TYPE PM_BLOCK IS ARRAY(0 TO 31) OF std_logic_vector(7 DOWNTO 0); -- program memory that will store the instructions sequentially from part 1 and part 2 test program
    
BEGIN
    PROCESS(clk_ctrl) -- complete the sensitivity list
    
        -- "PM" is the program memory that holds the instructions to be executed by the CPU 
        VARIABLE PM                      : PM_BLOCK;     
        -- Instruction once fetched will be stored in the IR register                   
        VARIABLE IR                      : std_logic_vector(7 DOWNTO 0);
        -- To decode the 4 MSBs from the PC content
        VARIABLE OPCODE                  : std_logic_vector( 3 DOWNTO 0);
        -- PC pointing to the program memory
        VARIABLE PC                      : integer RANGE 0 TO 31;
        -- Zero flag and positive flag
        VARIABLE zero_flag, positive_flag: std_logic;
        
        BEGIN
            IF (rst_ctrl='1') THEN -- RESET initializes all the control signals to 0.
                PC := 0;
                muxsel_ctrl <= "00";
                imm_ctrl <= (OTHERS => '0');
                accwr_ctrl <= '0';
                rfaddr_ctrl <= "000";
                rfwr_ctrl <= '0';
                alusel_ctrl <= "000";
                outen_ctrl <= '0';
                done       <= '0';
                state <= Fetch;    

-- *************** assembly code for PART1/PART2 goes here
--                PM(0) := "XXXXXXXX"; -- for example this is how the instructions will be stored in the program memory
-- **************

           ELSIF (clk_ctrl'event and clk_ctrl = '1') THEN
                CASE state IS
                    WHEN Fetch => -- fetch instruction
                                if(enter = '1')then
                                    PC_out <= conv_std_logic_vector(PC,5);
                                    IR := PM(PC);
                                    -- ****************************************
                                    -- write one line of code to get the opcode from the IR
                                    
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
                                    done       <= '0';
                                    state <= Decode;
                                elsif(enter = '0')then
                                    state <= Fetch;
                                end if;

                    WHEN Decode => -- decode instruction
                            CASE OPCODE IS
                                WHEN LDA => state   <= LDA_execute;
                                WHEN STA => state   <= STA_execute;
                                WHEN LDI => state   <= LDI_execute;
                                WHEN ADD => state   <= ADD_execute;
                                WHEN SUB => state   <= SUB_execute;
                                WHEN SHFL => state  <= SHFL_execute;
                                WHEN SHFR => state  <= SHFR_execute;
                                WHEN INA  => state  <= input_A;
                                WHEN OUTA => state  <= output_A;
                                WHEN HALT => state  <= Halt_cpu;
                                WHEN JZ   => state  <= JZ_execute;
                                WHEN OTHERS => state <= Halt_cpu;
                                
                            END CASE;
                            
                            muxsel_ctrl <= "00";
                            imm_ctrl <= (OTHERS => '0');
                            accwr_ctrl <= '0';
                            rfaddr_ctrl <= "000";
                            rfwr_ctrl <= '0';
                            alusel_ctrl <= "000";
                            outen_ctrl <= '0';
                            done       <= '0';
                            
                    WHEN flag_state => -- set zero and positive flags and then goto next instruction
                            muxsel_ctrl <= "00";
                            imm_ctrl <= (OTHERS => '0');
                            accwr_ctrl <= '0';
                            rfaddr_ctrl <= "000";
                            rfwr_ctrl <= '0';
                            alusel_ctrl <= "000";
                            outen_ctrl <= '0';
                            done       <= '0';
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
    
                    WHEN STA_execute => -- STA 
                            muxsel_ctrl <= "00";
                            imm_ctrl <= (OTHERS => '0');
                            accwr_ctrl <= '0';
                            rfaddr_ctrl <= IR(2 DOWNTO 0);
                            rfwr_ctrl <= '1';
                            alusel_ctrl <= "000";
                            outen_ctrl <= '0';
                            done       <= '0';
                            state <= Fetch;   
                            
                    WHEN LDI_execute => -- LDI 
                            -- *********************************
                            -- write the entire state for LDI_execute
                            
                            
                    WHEN JZ_execute => -- JZ
                            -- *********************************
                            -- write the entire state for JZ_execute

                   
                    WHEN ADD_execute => -- ADD 
                            -- *********************************
                            -- write the entire state for ADD_execute
 
                    WHEN SUB_execute => -- SUB 
                            -- *********************************
                            -- write the entire state for SUB_execute

                
                    WHEN SHFL_execute => -- SHFL
                            -- *********************************
                            -- write the entire state for SHFL_execute
                            
                    
                    WHEN SHFR_execute => -- SHFR 
                            -- *********************************
                            -- write the entire state for SHFR_execute

                    
                    WHEN input_A => -- INA
                            muxsel_ctrl <= "10";
                            imm_ctrl <= (OTHERS => '0');
                            accwr_ctrl <= '1';
                            rfaddr_ctrl <= "000";
                            rfwr_ctrl <= '0';
                            alusel_ctrl <= "000";
                            outen_ctrl <= '0';
                            done       <= '0';
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
                            done       <= '0';
                            state <= Halt_cpu;
    
                    WHEN OTHERS =>
                            muxsel_ctrl <= "00";
                            imm_ctrl <= (OTHERS => '0');
                            accwr_ctrl <= '0';
                            rfaddr_ctrl <= "000";
                            rfwr_ctrl <= '0';
                            alusel_ctrl <= "000";
                            outen_ctrl <= '0';
                            done       <= '0';
                            state <= Halt_cpu;
                END CASE;
        END IF;

    END PROCESS;
end Behavior;    
