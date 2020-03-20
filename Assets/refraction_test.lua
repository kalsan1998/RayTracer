-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material_refl({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25, 0.3)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
water = gr.material_refr({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 25, 0.7, 1.3)

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

-- the arc
arc = gr.node('arc')
scene:add_child(arc)
arc:translate(0,0,-10)
arc:rotate('Y', 60)
p1 = gr.cube('p1')
arc:add_child(p1)
p1:set_material(gold)
p1:scale(0.8, 4, 0.8)
p1:translate(-2.4, 0, -0.4)

p2 = gr.cube('p2')
arc:add_child(p2)
p2:set_material(gold)
p2:scale(0.8, 4, 0.8)
p2:translate(1.6, 0, -0.4)

s = gr.sphere('s')
arc:add_child(s)
s:set_material(gold)
s:scale(4, 0.6, 0.6)
s:translate(0, 4, 0)

-- the floor
sea = gr.mesh( 'sea', './Assets/plane.obj' )
scene:add_child(sea)
sea:set_material(water)
sea:scale(30, 30, 30)
sea:translate(0, 1, 0)

plane = gr.mesh( 'plane', './Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)


poly = gr.mesh( 'poly', './Assets/dodeca.obj' )
scene:add_child(poly)
poly:translate(-2, 1.618034, 0)
poly:set_material(blue)

sphere = gr.sphere('s')
scene:add_child(sphere)
sphere:translate(-5,0,0)
sphere:set_material(blue)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, './images/refraction.png', 512, 512, 
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})
