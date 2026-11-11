#!/usr/bin/env python3
"""Supplemental ARM link check. Keil remains the primary project."""
from pathlib import Path
import os,subprocess,xml.etree.ElementTree as E
root=Path(__file__).resolve().parent.parent
os.chdir(root)
prefix=os.environ.get('ARM_GCC_PREFIX','arm-none-eabi-')
cc=prefix+'gcc';out=root/'build/gcc';out.mkdir(parents=True,exist_ok=True)
t=E.parse('MDK-ARM/Tetris_Joystick.uvprojx');base=root/'MDK-ARM'
includes=next(n.text for n in t.iter('IncludePath') if n.text)
inc=[str((base/p).resolve()) for p in includes.split(';') if p and 'portable/RVDS' not in p]
inc.insert(0,str(root/'tools/gcc_port'))
flags=['-mcpu=cortex-m3','-mthumb','-std=c99','-Os','-g3','-ffunction-sections','-fdata-sections','-fstack-usage','-Wall','-Wextra','-DUSE_HAL_DRIVER','-DSTM32F103xB']+['-I'+p for p in inc]
# Debian packages extracted without installing need explicit newlib locations.
if os.environ.get('ARM_NEWLIB_ROOT'):
 n=Path(os.environ['ARM_NEWLIB_ROOT']);flags+=['-I'+str(n/'../../include/newlib'),'-B'+str(n/'newlib/thumb/v7-m/nofp')+'/']
sources=[(base/n.text).resolve() for n in t.findall('.//Group//FilePath') if n.text.endswith('.c') and '/portable/RVDS/' not in n.text]
sources += [root/'tools/gcc_port/port.c',root/'tools/startup_gcc.S']
objs=[]
for i,s in enumerate(sources):
 obj=out/(str(i)+'_'+s.stem+'.o');subprocess.run([cc,*flags,'-c',str(s),'-o',str(obj)],check=True);objs.append(str(obj))
subprocess.run([cc,*flags,*objs,'-Ttools/stm32f103c8.ld','-nostartfiles','--specs=nano.specs','--specs=nosys.specs','-Wl,--gc-sections,-Map='+str(out/'tetris.map'),'-o',str(out/'tetris.elf')],check=True)
subprocess.run([prefix+'size',str(out/'tetris.elf')],check=True)
subprocess.run([prefix+'objcopy','-O','ihex',str(out/'tetris.elf'),str(out/'tetris.hex')],check=True)
