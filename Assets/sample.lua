-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

brown = gr.material({0.5, 0.3, 0.1}, {0.8, 0.8, 0.4}, 10)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.8, 0.8, 0.8}, 45)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
mirror_m = gr.material_ext({0,0,0}, {0,0,0}, 1, 1.0)

scene = gr.node('scene')
scene:rotate('X', 8)
scene:translate(6, -2, -15)

mirror = gr.node('mirror')
scene:add_child(mirror)
mirror:rotate('Y', 60)
mirror:scale(3.25, 2, 0.5)
mirror:translate(-6,0,-15)

p1 = gr.cube('p1')
mirror:add_child(p1)
p1:set_material(brown)
p1:scale(0.2, 4, 0.5)
p1:translate(-4.4, 0, -0.4)

p2 = gr.cube('p2')
mirror:add_child(p2)
p2:set_material(brown)
p2:scale(0.2, 4, 0.5)
p2:translate(3.6, 0, -0.4)

s = gr.cube('s')
mirror:add_child(s)
s:set_material(brown)
s:scale(8.2, 0.3, 0.5)
s:translate(-4.4, 4, -0.4)

mir = gr.cube('mir')
mirror:add_child(mir)
mir:set_material(mirror_m)
mir:scale(8, 4, 0.4)
mir:translate(-4.3, 0, -0.35)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(70, 70, 70)

cow_poly = gr.mesh('cow', 'cow.obj')
cow_poly:rotate('Y', 160)
factor = 2.0/(2.76+3.637)

cow_poly:set_material(hide)

cow_poly:translate(0.0, 3.637, 0.0)
cow_poly:scale(factor, factor, factor)
cow_poly:translate(0.0, -1.0, 0.0)

scene:add_child(cow_poly)
cow_poly:scale(1.4, 1.4, 1.4)
cow_poly:translate(-2, 1.3, -10)

cow = gr.node('the_cow')

for _, spec in pairs({
			{'body', {0, 0, 0}, 1.0},
			{'head', {.9, .3, 0}, 0.6},
			{'tail', {-.94, .34, 0}, 0.2},
			{'lfleg', {.7, -.7, -.7}, 0.3},
			{'lrleg', {-.7, -.7, -.7}, 0.3},
			{'rfleg', {.7, -.7, .7}, 0.3},
			{'rrleg', {-.7, -.7, .7}, 0.3}
		     }) do
   part = gr.nh_sphere(table.unpack(spec))
   part:set_material(hide)
   cow:add_child(part)
end

-- Use the instanced cow model to place some actual cows in the scene.
-- For convenience, do this in a loop.

cow_number = 1

for _, pt in pairs({
		      {{0,1.3,2}, 190},
		      {{-10,1.3,2}, 180},
			  {{-5.5,1.3,-3}, 150},
			  {{1, 1.3, -5}, 60}}) do
   cow_instance = gr.node('cow' .. tostring(cow_number))
   scene:add_child(cow_instance)
   cow_instance:add_child(cow)
   cow_instance:scale(1.4, 1.4, 1.4)
   cow_instance:rotate('Y', pt[2])
   cow_instance:translate(table.unpack(pt[1]))
   
   cow_number = cow_number + 1
end

-- The lights
l1 = gr.light({-200, 100, 100}, {0.8, 0.8, 0.8}, {0.9, 0, 0})

gr.render(scene, 'sample.png', 1024, 1024, 
	  {0, 4, 0}, {0, -0.25, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2})
