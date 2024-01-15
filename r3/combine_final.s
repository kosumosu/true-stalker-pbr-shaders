function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("stub_notransform_t","combine_final")
			: fog	(false)
			: zb 	(false,false)
	shader:dx10texture	("s_base", "$user$generic")

	shader:dx10sampler	("smp_rtlinear");
end
