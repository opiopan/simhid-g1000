#!/usr/bin/env python3
import os, sys, gerberex
from gerberex import DxfFile, GerberComposition, DrillComposition
sys.path.append(os.path.join(os.path.dirname(__file__), '../../pcbalib'))
import jlcpcb

exts = ['GTL', 'GTO', 'GTP', 'GTS', 'GBL', 'GBO', 'GBP', 'GBS', 'TXT']
boards=[
    ('../individuals/CAMOutputs/simhid-g1000_left.', 0, 0, 0),
    ('../individuals/CAMOutputs/simhid-g1000_right.', 57.05, 0, 0),
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
ctx = jlcpcb.Composition()
ctx.setBom('BOM.csv')
for board in boards:
    print('.', end='', flush=True)
    file = jlcpcb.MountFile()
    file.load(board[0] + 'mnt', jlcpcb.TOP)
    file.rotate(board[3])
    file.offset(board[1], board[2])
    ctx.merge(file)
    file = jlcpcb.MountFile()
    file.load(board[0] + 'mnb', jlcpcb.BOTTOM)
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
base = gerberex.rectangle(width=100, height=100, left=0, bottom=0, units='metric')
base.draw_mode = DxfFile.DM_FILL
ctx.merge(base)
file.to_metric()
file.draw_mode = DxfFile.DM_FILL
file.negate_polarity()
ctx.merge(file)
ctx.dump(outputs + '-fill.GML')

print('. end', flush=True)
