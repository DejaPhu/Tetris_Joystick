"""Validate Keil references and CubeMX object counts without running Keil."""
from pathlib import Path
import xml.etree.ElementTree as E
import re
r=Path(__file__).resolve().parent.parent
base=r/'Teris/MDK-ARM';t=E.parse(base/'Teris.uvprojx')
files=[(base/n.text).resolve() for n in t.findall('.//Group//FilePath')]
assert len(files)==len(set(files))
for f in files:assert f.is_file(),f
for n in t.iter('IncludePath'):
 for p in (n.text or '').split(';'):
  if p:assert (base/p).is_dir(),p
for name,start,size in [('IROM','0x08000000','0x10000'),('IRAM','0x20000000','0x5000')]:
 n=t.find('.//OnChipMemories/'+name);assert int(n.findtext('StartAddress'),16)==int(start,16);assert n.findtext('Size')==size
s=(r/'main.c').read_text()
assert len(re.findall(r'\bosThreadNew\s*\(',s))==3
assert len(re.findall(r'\bosMessageQueueNew\s*\(',s))==2
assert len(re.findall(r'\bosKernelStart\s*\(',s))==1
for name in ['input','game','display']:assert 'task_'+name+'_run(argument);' in s
for folder in ['app/src','hal/src']:
 for f in (r/folder).glob('*.c'):assert f.resolve() in files;assert 'HAL_Delay(' not in f.read_text()
print('PASS: Keil source/include/memory references; exactly 3 application Tasks, 2 queues, 1 kernel start')

for folder in ["bsp/inc","bsp/src","hal/inc","hal/src","middlewares/FreeRTOS","app/inc","app/src"]:assert (r/folder).is_dir()

irq=(r/'bsp/src/stm32f1xx_it.c').read_text()
assert len(re.findall(r'void USART1_IRQHandler\s*\(void\)',irq))==1
assert 'uart_debug_irq();' in irq
print('PASS: USART1 handler linked to nonblocking debug driver; no new RTOS objects')

conf=(r/'bsp/inc/stm32f1xx_hal_conf.h').read_text()
assert re.search(r'^\s*#define HAL_ADC_MODULE_ENABLED\s*$',conf,re.M)
ioc=dict(line.split('=',1) for line in (r/'Teris/Teris.ioc').read_text().splitlines() if '=' in line)
assert ioc['RCC.ADCPresc']=='RCC_ADCPCLK2_DIV6'
assert ioc['RCC.ADCFreqValue']=='12000000'
assert ioc['ADC1.Channel-0\\#ChannelRegularConversion']=='ADC_CHANNEL_0'
assert ioc['ADC2.Channel-0\\#ChannelRegularConversion']=='ADC_CHANNEL_1'
for adc in ['ADC1','ADC2']:
 assert ioc[adc+'.SamplingTime-0\\#ChannelRegularConversion']=='ADC_SAMPLETIME_55CYCLES_5'
 assert 'HAL_ADCEx_Calibration_Start(&hadc'+adc[-1]+')' in s
print('PASS: ADC HAL enabled, CubeMX channels/sampling/12MHz clock, both ADCs calibrated')

assert ioc['PB6.Signal']=='GPIO_Output'
assert ioc['PB6.PinState']=='GPIO_PIN_SET'
assert 'MX_TIM4_Init' not in s
assert s.index('HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)') < s.index('HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct)')
print('PASS: active-low buzzer PB6 GPIO, startup HIGH preload, no TIM4 PWM init')
