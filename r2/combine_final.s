function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("null","combine_final")
			: fog	(false)
			: zb 	(false,false)
	shader:sampler	("s_base")	:texture("$user$generic") : clamp() : f_linear ()
end
