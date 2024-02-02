#ifndef	common_defines_h_included
#define	common_defines_h_included

//////////////////////////////////////////////////////////////////////////////////////////
// Defines                                		//
#define def_gloss       float(2.f /255.f)
#define def_aref        float(200.f/255.f)
#define def_grass_aref  float(170.f/255.f)
#define def_dbumph      float(0.333f)
#define def_virtualh    float(0.05f)              // 5cm
#define def_distort     float(0.05f)             // we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr         float(18.h)         		// hight luminance range float(3.h)
#define def_hdr_clip	float(0.75h)        		//

#define	LUMINANCE_VECTOR	float3(0.3f, 0.38f, 0.22f)

#define SKY_EPS		float(0.001)
#define MAT_TERRAIN	float(0.95)
#define MAT_FLORA	float(0.15)
#define MAT_FLORA_ELIPSON float(0.04)

//#define SSFX_FLORAFIX

//////////////////////////////////////////////////////////////////////////////////////////
#ifndef SMAP_size
#define SMAP_size        1024
#endif
//////////////////////////////////////////////////////////////////////////////////////////

#endif	//	common_defines_h_included