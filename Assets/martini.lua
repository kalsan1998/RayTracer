-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

table = gr.material({0.3, 0.1, 0.1}, {0.0, 0.0, 0.0}, 0)
water = gr.material_refr({0,0,0}, {0,0,0}, 0, 1.0, 1.3)

scene = gr.node('scene')
scene:rotate('X', 13)
scene:translate(0, -4, -15)

-- the floor
plane = gr.mesh( 'plane', './Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(table)
plane:scale(30, 30, 30)

cup = gr.cylinder('cup')
scene:add_child(cup)
cup:set_material(table)
cup:scale(5,5,5)


-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, './images/martini.png', 512, 512, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})