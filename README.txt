ee5301-mp3

Tonglin Chen chen5202@umn.edu

Execution command: ./floorplan -<option> [filename]
available options are -a optimizing based on area
	  	      -w optimizing based on wire length
		      -c optimizing based on both area and wire length

The output file would be in the same directory as the input file.
Information on each step will be available in the same directory as
the program. The name would be "step.csv".

floorplan.cpp:  main function, timer and IO

util.cpp/hpp: 	annealing engine and file parsing function

module.cpp/hpp:	class definition for each module and class methods

graph.cpp/hpp:	class definition for HCG and VCG constructing from
		Sequence Pair and calculating the floorplan from
		HCG and VCG

Approach:

When building the HCG and VCG, an adjacent matrix is used for storing
the graph. There is a benefit using adjacent matrix instead of adjacent
list: when changing two elements from SP (assume they are i and j), the
modification for HCG and VCG would be all possible edges containing i and
j. Since the position of other elements are not changed, other edge will not
be changed. So the algorithm for updating a graph would be:

For Each m belongs to vertices,
    update (m, i), (i, m), (m, j), (j, m)

Using adjacent matrix, modifing (m,i) requires O(1) time (array can be
random accessed) while adjacent list will cost worst case O(n) time.XS

For different options, cost function is essentially the same, except
that the alpha value is changing. For a, alpha = 1, for w, alpha = 0.
For c, alpha is a preset value and currently set to 0.5.

The effect of k, T, and cool_rate is well discussed in MP2 so I'm setting
Tstart = 4e4, Tfreeze = 1e-1, cool_rate = 0.95 and k set to initial
accept rate at 0.95.

Detailed performance will be shown in enclosed pdf.
