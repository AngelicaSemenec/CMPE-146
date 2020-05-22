################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/semen/workspace_v9/Lab7_1_RTOS" --include_path="C:/Users/semen/workspace_v9/Lab7_1_RTOS/Debug" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source/third_party/CMSIS/Include" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source/ti/posix/ccs" --include_path="C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --advice:power=none -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/semen/workspace_v9/Lab7_1_RTOS/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1948366305:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1948366305-inproc

build-1948366305-inproc: ../hello.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/ccs930/xdctools_3_60_02_34_core/xs" --xdcpath="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source;C:/ti/simplelink_msp432p4_sdk_3_40_00_05/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.msp432:MSP432P401R -r release -c "C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path=\"C:/Users/semen/workspace_v9/Lab7_1_RTOS\" --include_path=\"C:/Users/semen/workspace_v9/Lab7_1_RTOS/Debug\" --include_path=\"C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source\" --include_path=\"C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source/third_party/CMSIS/Include\" --include_path=\"C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source/ti/posix/ccs\" --include_path=\"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include\" --advice:power=none -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1948366305 ../hello.cfg
configPkg/compiler.opt: build-1948366305
configPkg/: build-1948366305

build-365826705:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-365826705-inproc

build-365826705-inproc: ../hello.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs930/ccs/utils/sysconfig/sysconfig_cli.bat" -s "C:/ti/simplelink_msp432p4_sdk_3_40_00_05/.metadata/product.json" -o "syscfg" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_drivers_config.c: build-365826705 ../hello.syscfg
syscfg/ti_drivers_config.h: build-365826705
syscfg/syscfg_c.rov.xs: build-365826705
syscfg/: build-365826705

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/semen/workspace_v9/Lab7_1_RTOS" --include_path="C:/Users/semen/workspace_v9/Lab7_1_RTOS/Debug" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source/third_party/CMSIS/Include" --include_path="C:/ti/simplelink_msp432p4_sdk_3_40_00_05/source/ti/posix/ccs" --include_path="C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --advice:power=none -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/semen/workspace_v9/Lab7_1_RTOS/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


