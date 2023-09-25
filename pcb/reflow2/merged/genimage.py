#!/usr/bin/env python
from gerber import load_layer
from gerber.render import RenderSettings, theme
from gerber.render.cairo_backend import GerberCairoContext

prefix = 'outputs/simhid-g1000'

ctx = GerberCairoContext(scale=40)

print('loading ', end='', flush=True)
copper = load_layer(prefix + '.GTL')
print('.', end='', flush=True)
mask = load_layer(prefix + '.GTS')
print('.', end='', flush=True)
silk = load_layer(prefix + '.GTO')
print('.', end='', flush=True)
drill = load_layer(prefix + '.TXT')
print('.', end='', flush=True)
outline = load_layer(prefix + '-fill.GML')
print('.', end='', flush=True)
print('. end', flush=True)
 
print('drawing ', end='', flush=True)
print('.', end='', flush=True)

metal_settings = RenderSettings(color=(30.0/255.0, 119.0/255.0, 93/255.0))
bg_settings = RenderSettings(color=(30.0/300.0, 110.0/300.0, 93/300.0))
ctx.render_layer(copper, settings=metal_settings, bgsettings=bg_settings)
print('.', end='', flush=True)

copper_settings = RenderSettings(color=(0.7*1.2, 0.5*1.2, 0.1*1.2))
ctx.render_layer(mask, settings=copper_settings)
print('.', end='', flush=True)

our_settings = RenderSettings(color=theme.COLORS['white'], alpha=0.80)
ctx.render_layer(silk, settings=our_settings)
print('.', end='', flush=True)

ctx.render_layer(outline)
print('.', end='', flush=True)
ctx.render_layer(drill)
print('.', end='', flush=True)
print('. end', flush=True)

print('dumping ...')
ctx.dump('outputs/board-top.png')

ctx.clear()
print('loading bottom ', end='', flush=True)
copper = load_layer(prefix + '.GBL')
print('.', end='', flush=True)
mask = load_layer(prefix + '.GBS')
print('.', end='', flush=True)
silk = load_layer(prefix + '.GBO')
print('.', end='', flush=True)
print('. end', flush=True)

print('drawing bottom ', end='', flush=True)
ctx.render_layer(copper, settings=metal_settings, bgsettings=bg_settings)
print('.', end='', flush=True)
ctx.render_layer(mask, settings=copper_settings)
print('.', end='', flush=True)
ctx.render_layer(silk, settings=our_settings)
print('.', end='', flush=True)
ctx.render_layer(outline)
print('.', end='', flush=True)
ctx.render_layer(drill)
print('.', end='', flush=True)
print('. end', flush=True)

print('dumping bottom ...')
ctx.dump('outputs/board-bottom.png')
