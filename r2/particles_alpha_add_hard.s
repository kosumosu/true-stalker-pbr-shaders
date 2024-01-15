function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("particle",	"particle_hard")
			: sorting	(3, false)
			: blend		(true,blend.srcalpha,blend.one)
			: aref 		(false,0)
			: zb 		(true,false)
			: fog		(false)

	shader:sampler	("s_base")      :texture	(t_base)
	shader:sampler  ("s_position")  :texture  ("$user$position")
end
