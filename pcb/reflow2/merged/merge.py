#!/usr/bin/env python3
import os, sys, gerberex
from gerberex import DxfFile, GerberComposition, DrillComposition
sys.path.append(os.path.join(os.path.dirname(__file__), '../../pcbalib'))
import jlcpcb

exts = ['GTL', 'GTO', 'GTP', 'GTS', 'GBL', 'GBO', 'GBP', 'GBS', 'TXT']
additional_parts_right = [
    jlcpcb.Component('EC8', (15.0, 78.401), 0.0, jlcpcb.TOP),
]
boards=[
    ('../individuals/CAMOutputs/simhid-g1000_left.', 0, 0, 0),
    ('../individuals/CAMOutputs/simhid-g1000_right.', 57.05, 0, 0, additional_parts_right),
    ('../individuals/CAMOutputs/simhid-g1000_bottom.', 40.05, 224, -90),
]
outline = 'outline.dxf'
mousebites = 'mousebites.dxf'
outputs = 'outputs/simhid-g1000'
cpl = 'outputs/CPL.csv'

if not os.path.isdir('outputs'):
    os.mkdir('outputs')

for ext in exts:
    print('merging %s: ' % ext ,end='', flush=True)
    if ext == 'TXT':
        ctx = DrillComposition()
    else:
        ctx = GerberComposition()
    for board in boards:
        file = gerberex.read(board[0] + ext)
        file.to_metric()
        if ext == 'GTO' or ext == 'GBO':
            for adef in file.aperture_defs:
                if adef.shape == 'C' and adef.modifiers[0][0] < 0.12:
                    adef.modifiers[0] = (0.12,)
                elif adef.shape == 'R' and adef.modifiers[0][1] < 0.05:
                    adef.modifiers[0] = (adef.modifiers[0][0], 0.05)
        file.rotate(board[3])
        file.offset(board[1], board[2])
        ctx.merge(file)
        print('.', end='', flush=True)
    if ext == 'TXT':
        if mousebites:
            file = gerberex.read(mousebites)
            file.draw_mode = DxfFile.DM_MOUSE_BITES
            file.width = 0.5
            file.format = (3, 3)
            ctx.merge(file)
    else:
        file = gerberex.read(outline)
        ctx.merge(file)
    ctx.dump(outputs + '.' + ext)
    print(' end', flush=True)

print('generating CPL: ', end='', flush=True)
offset_angles = {
    'SOT-23-3_L2.9-W1.3-P1.90-LS2.4-BR': -90,
    'QFN-48_L7.0-W7.0-P0.50-BL-EP5.1': -90,
    'QFN-28_L6.0-W6.0-P0.65-BL-EP3.7': -90,
    'SW-TH_EC12D1524403': ((0.15, 1.35), 180),
    'SW-TH_EC11EBB24C03': ((0, 1), 180),
    'SW-SMD_SLLB510100': ((0, 0.4), 0)
}
ctx = jlcpcb.Composition()
ctx.setBom('BOM.csv', offset_angles)
for board in boards:
    print('.', end='', flush=True)
    file = jlcpcb.MountFile()
    file.load(board[0] + 'mnt', jlcpcb.TOP)
    if len(board) > 4:
        file.append(board[4])
    file.rotate(board[3])
    file.offset(board[1], board[2])
    ctx.merge(file)
    file = jlcpcb.MountFile()
    file.load(board[0] + 'mnb', jlcpcb.BOTTOM, exclude=['EC1','EC2','EC3','EC4','EC5','EC6','EC7','EC9'])
    file.rotate(board[3])
    file.offset(board[1], board[2])
    ctx.merge(file)
ctx.dump(cpl)
print(' end', flush=True)
    
print('generating GML: ', end='', flush=True)
file = gerberex.read(outline)
file.write(outputs + '.GML')
print('.', end='', flush=True)
ctx = GerberComposition()
base = gerberex.rectangle(width=320, height=320, left=0, bottom=0, units='metric')
base.draw_mode = DxfFile.DM_FILL
ctx.merge(base)
file.to_metric()
file.draw_mode = DxfFile.DM_FILL
file.negate_polarity()
ctx.merge(file)
ctx.dump(outputs + '-fill.GML')

print('. end', flush=True)
