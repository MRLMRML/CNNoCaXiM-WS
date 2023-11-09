#pragma once
//#define DEBUG
//#define DEBUGROUTER
//#define MEASURE
#define CLOCK

#define DATA_PRECISION float

//////////////////////// Terminal parameters ////////////////////////
// DRAM
#define WEIGHT_DATA_START_LOCATION 0
#define WEIGHT_DATA_LINE_COUNT 10
#define INPUT_DATA_START_LOCATION 10
#define INPUT_DATA_LINE_COUNT 10
#define OUTPUT_DATA_START_LOCATION 20

// Controller
#define WEIGHT_DIMENSION 3
#define INPUT_DIMENSION 5
#define OUTPUT_DIMENSION INPUT_DIMENSION - WEIGHT_DIMENSION + 1

//////////////////////// Timing ////////////////////////
// Controller
#define EXECUTION_TIME_CONTROLLER_NW 1
#define EXECUTION_TIME_CONTROLLER_WI 1
#define EXECUTION_TIME_CONTROLLER_IO 1
#define EXECUTION_TIME_CONTROLLER_OO 1
#define EXECUTION_TIME_CONTROLLER_OB 1
#define EXECUTION_TIME_CONTROLLER_BI 1

// DRAM timing
#define EXECUTION_TIME_DRAM_WI 1 // DRAM read weight time
#define EXECUTION_TIME_DRAM_IO 1 // DRAM read input time
#define EXECUTION_TIME_DRAM_OI 1 // DRAM write output time

// PE timing
#define EXECUTION_TIME_PE_WI 1
#define EXECUTION_TIME_PE_II 1 // PE calculation time, not used

// NI timing
#define EXECUTION_TIME_NI_WI 1
#define EXECUTION_TIME_NI_IO 1
#define EXECUTION_TIME_NI_OI 1

//////////////////////// NoC parameters ////////////////////////
// Network dimension
#define NETWORK_DIMENSION_X 2
#define NETWORK_DIMENSION_Y 1
#define ROUTER_NUMBER NETWORK_DIMENSION_X * NETWORK_DIMENSION_Y
// choose network type:
#define MESH
//#define TORUS

// Virtual channel
#define VC_NUMBER 8
#define BUFFER_SIZE 8 
#define VIRTUAL_CHANNEL_PRIORITY_SEQUENCIAL
//#define VIRTUAL_CHANNEL_PRIORITY_SNAKE
#define NETWORK_BANDWIDTH 1 // flit per cycle
#define REGISTER_DEPTH 2 // to assist parallelism with "enable"

// Packet and Flit size
#define PACKET_SIZE 20 // bytes
#define FLIT_SIZE 1 // bytes, link width
#define FLIT_NUMBER_PER_PACKET PACKET_SIZE / FLIT_SIZE

// Packet number to send in total
#define PACKET_NUMBER 30

// Traffic pattern
//#define CUSTOMIZE
//#define PERMUTATION
#define DRAIN_NODE 2
#define UNIFORM

// Injection rate
//#define PACKET_PER_CYCLE
#define FLIT_PER_CYCLE
#define INJECTION_RATE 0.05f

//////////////////////// Timing ////////////////////////
// Router timing, fixed, might support other values even though not tested yet
#define EXECUTION_TIME_ROUTER_RC 1.0f
#define PERIOD_ROUTER_RC 1.0f
#define EXECUTION_TIME_ROUTER_VA 1.0f
#define PERIOD_ROUTER_VA 1.0f
#define EXECUTION_TIME_ROUTER_SA 1.0f
#define PERIOD_ROUTER_SA 1.0f
#define EXECUTION_TIME_ROUTER_ST 1.0f
#define PERIOD_ROUTER_ST 1.0f

// Link timing, fixed, PERIOD = EXECUTION_TIME = 1, need to change link behaviour to support higher value
#define EXECUTION_TIME_LINK 1.0f 
#define PERIOD_LINK EXECUTION_TIME_LINK

// Node timing, PERIOD >= EXECUTION_TIME
#define EXECUTION_TIME_DOPPLERNODE_INJECTTRAFFIC 1.0f
#if defined (PACKET_PER_CYCLE)
#define PERIOD_DOPPLERNODE_INJECTTRAFFIC static_cast<float>(1 / INJECTION_RATE)
#endif
#if defined (FLIT_PER_CYCLE)
#define PERIOD_DOPPLERNODE_INJECTTRAFFIC static_cast<float>(FLIT_NUMBER_PER_PACKET / INJECTION_RATE)
#endif
#define EXECUTION_TIME_DOPPLERNODE_COLLECTTRAFFIC 1.0f
#define PERIOD_DOPPLERNODE_COLLECTTRAFFIC EXECUTION_TIME_DOPPLERNODE_COLLECTTRAFFIC

// Simulation cycles
#define SIMULATION_CYCLES 1500
#define WARMUP_CYCLES 100
#define MEASUREMENT_CYCLES 400
#define DRAIN_CYCLES SIMULATION_CYCLES - WARMUP_CYCLES - MEASUREMENT_CYCLES

