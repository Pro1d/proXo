
/// Configuration

#define CPU_CORES_COUNT		8
#define THREADS_COUNT        CPU_CORES_COUNT
#define ENABLE_CEL_SHADING  true


/// Optimization / precision

// Comment/Uncomment
//#define USE_DOUBLE_PRECISION_FLOATING_VALUE
#define USE_64_BITS_INTEGER_VALUE

#define FASTEST_INV_SQRT		0
#define NEWTON_INV_SQRT			1
#define NEWTON2_INV_SQRT		2
#define MULT_NEWTON_INV_SQRT	3
#define DEFAULT_MATH_INV_SQRT	4
#define INV_SQRT_METHOD			NEWTON_INV_SQRT
