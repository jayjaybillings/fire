pythonoce-test.py timing results. 100000 points in a 10x10x10 box in a 20x20x20 universe.

Initial version, full display: 26.9s
Initial version, no display: 24.6s
Initial version, no display or stdout: 19.24s
Numpy version, no display: 21.8s
Numpy version, no display or stdout: 19.543s
Numpy version, same + direct containment insert: 19.747s
Numpy version, same + pandas: 19.6s
Pure pandas, otherwise same, no initial data: >800s
Pure pandas, otherwise same, initialized to zero: 165.3s
Pure pandas, same, 4 set_value instead of iloc (used profiler): 21.07s 

Note: vertices.set_value(i,['x','y','z','inShape'],[x,y,z,inShape]) does not perform well.

Profile command: time python -m cProfile pythonoce-test.py | grep "0000 "

Next to final version:
===
    10000    0.010    0.000    0.067    0.000 BRepBuilderAPI.py:2875(__init__)
    10000    0.012    0.000    1.996    0.000 BRepExtrema.py:201(__init__)
    10000    0.025    0.000    0.034    0.000 base.py:1351(_convert_slice_indexer)
    10000    0.008    0.000    0.024    0.000 base.py:1584(__iter__)
    10000    0.025    0.000    0.120    0.000 base.py:404(_shallow_copy)
    10000    0.024    0.000    0.033    0.000 base.py:701(_get_attributes_dict)
    10000    0.007    0.000    0.010    0.000 base.py:703(<listcomp>)
    10000    0.010    0.000    0.041    0.000 common.py:182(is_bool_indexer)
    10000    0.008    0.000    0.013    0.000 generic.py:1607(_indexer)
    10000    0.009    0.000    0.051    0.000 generic.py:3171(_is_mixed_type)
    10000    0.007    0.000    0.028    0.000 generic.py:3173(<lambda>)
    10000    0.011    0.000    0.057    0.000 gp.py:11046(__init__)
    10000    0.026    0.000    0.382    0.000 indexing.py:144(_get_setitem_indexer)
    10000    0.017    0.000    0.124    0.000 indexing.py:1583(_has_valid_type)
    10000    0.006    0.000    0.096    0.000 indexing.py:1602(_has_valid_setitem_indexer)
    10000    0.013    0.000    0.063    0.000 indexing.py:1632(_is_valid_integer)
    10000    0.063    0.000   13.492    0.001 indexing.py:173(__setitem__)
    10000    0.015    0.000    0.052    0.000 indexing.py:1863(length_of_indexer)
    10000    0.056    0.000    0.314    0.000 indexing.py:210(_convert_tuple)
    10000    0.019    0.000    0.078    0.000 indexing.py:238(_convert_slice_indexer)
    10000    0.040    0.000    0.090    0.000 indexing.py:246(_has_valid_positional_setitem_indexer)
    10000    0.319    0.000   13.011    0.001 indexing.py:273(_setitem_with_indexer)
    10000    0.004    0.000    0.015    0.000 indexing.py:519(can_do_equal_len)
    10000    0.010    0.000    0.025    0.000 internals.py:1806(_can_hold_element)
    10000    0.007    0.000    0.009    0.000 internals.py:1818(should_store)
    10000    0.009    0.000    0.021    0.000 internals.py:3309(is_mixed_type)
    10000    0.017    0.000    0.017    0.000 {built-in method OCC._BRepBuilderAPI.BRepBuilderAPI_MakeVertex_swiginit}
    10000    0.040    0.000    0.040    0.000 {built-in method OCC._BRepBuilderAPI.new_BRepBuilderAPI_MakeVertex}
    10000    0.039    0.000    0.039    0.000 {built-in method OCC._BRepExtrema.BRepExtrema_DistShapeShape_swiginit}
    10000    1.945    0.000    1.945    0.000 {built-in method OCC._BRepExtrema.new_BRepExtrema_DistShapeShape}
    10000    0.027    0.000    0.027    0.000 {built-in method OCC._gp.gp_Pnt_swiginit}
    10000    0.019    0.000    0.019    0.000 {built-in method OCC._gp.new_gp_Pnt}

Final version
===
    10000    0.006    0.000    0.043    0.000 BRepBuilderAPI.py:2875(__init__)
    10000    0.010    0.000    1.782    0.000 BRepExtrema.py:201(__init__)
    40000    0.061    0.000    0.201    0.000 frame.py:1832(set_value)
    10000    0.008    0.000    0.034    0.000 gp.py:11046(__init__)
    10000    0.012    0.000    0.012    0.000 {built-in method OCC._BRepBuilderAPI.BRepBuilderAPI_MakeVertex_swiginit}
    10000    0.024    0.000    0.024    0.000 {built-in method OCC._BRepBuilderAPI.new_BRepBuilderAPI_MakeVertex}
    10000    0.034    0.000    0.034    0.000 {built-in method OCC._BRepExtrema.BRepExtrema_DistShapeShape_swiginit}
    10000    1.739    0.000    1.739    0.000 {built-in method OCC._BRepExtrema.new_BRepExtrema_DistShapeShape}
    10000    0.016    0.000    0.016    0.000 {built-in method OCC._gp.gp_Pnt_swiginit}
    10000    0.009    0.000    0.009    0.000 {built-in method OCC._gp.new_gp_Pnt}
    30000    0.004    0.000    0.004    0.000 {method 'random' of '_random.Random' objects}
    40000    0.060    0.000    0.060    0.000 {method 'set_value' of 'pandas._libs.index.IndexEngine' objects}

