################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
MIL/%.obj: ../MIL/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="C:/Users/jacks/workspace_v10/ServoController" --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="C:/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --define=ccs="ccs" --define=PART_TM4C129XNCZAD --define=TARGET_IS_TM4C129_RA0 -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="MIL/$(basename $(<F)).d_raw" --obj_directory="MIL" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


