#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

#include "SobelFilter.h"

SobelFilter::SobelFilter( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	HLS_FLATTEN_ARRAY(val);
#endif
	SC_THREAD( do_filter );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
        
#ifndef NATIVE_SYSTEMC
	i_rgb.clk_rst(i_clk, i_rst);
  o_result.clk_rst(i_clk, i_rst);
#endif
}

SobelFilter::~SobelFilter() {}

const int mask[3][3] = {{1, 1, 1}, {1, 2, 1}, {1, 1, 1}};

void SobelFilter::do_filter() {
	{
#ifndef NATIVE_SYSTEMC
		HLS_DEFINE_PROTOCOL("main_reset");
		i_rgb.reset();
		o_result.reset();
#endif
		wait();
	}
	while (true) {
		for (unsigned int v = 0; v<height; ++v) {
			int median_count = 0;
			for (unsigned int u = 0; u<width; ++u) {
				sc_dt::sc_uint<24> rgb;
/*#ifndef NATIVE_SYSTEMC
				{
					HLS_DEFINE_PROTOCOL("input");
					rgb = i_rgb.get();
					wait();
				}
#else*/
				rgb = i_rgb.read();
#endif
				unsigned char grey = (rgb.range(7,0) + rgb.range(15,8) + rgb.range(23, 16))/3;
				for (unsigned int i = 0; i != MASK_N; ++i) {
					HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
					val[i] += grey * mask[i][u][v];
				}
			}
		}
		int total = 0;
		for (unsigned int i = 0; i != MASK_N; ++i) {
			HLS_CONSTRAIN_LATENCY(0, 1, "lat01");
			total += val[i] * val[i];
		}
/*#ifndef NATIVE_SYSTEMC
		{
			HLS_DEFINE_PROTOCOL("output");
			o_result.put(total);
			wait();
		}
#else*/
		o_result.write(total);
#endif
	}
}
