local tex_base                = "water\\water_studen"
local tex_nmap                = "water\\water_normal"
local tex_dist                = "water\\water_dudv"
local tex_env0                = "$user$sky0"         -- "sky\\sky_8_cube"
local tex_env1                = "$user$sky1"         -- "sky\\sky_8_cube"
local tex_leaves              = "water\\water_foam"

function normal                (shader, t_base, t_second, t_detail)
  shader:begin					("water_soft","water_soft")
        : sorting				(2, false)
        : blend					(true,blend.srcalpha,blend.invsrcalpha)
		: zb					(true,false)
		: fog					(true)
		: landscape				(true)
  shader:sampler        ("s_base")       :texture  (tex_base)
  shader:sampler        ("s_nmap")       :texture  (tex_nmap)
  shader:sampler        ("s_env0")       :texture  (tex_env0)   : clamp()
  shader:sampler        ("s_env1")       :texture  (tex_env1)   : clamp()
  shader:sampler        ("s_tonemap")    :texture  ("$user$tonemap")
  shader:sampler        ("s_position")       :texture  ("$user$position") : f_none ()
  shader:sampler        ("s_image")	   :texture  ("$user$generic0")	: clamp() : f_none ()
  shader:sampler        ("s_leaves")     :texture  (tex_leaves) : wrap() :f_anisotropic()
end