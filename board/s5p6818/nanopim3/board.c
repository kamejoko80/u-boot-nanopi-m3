/*
 * (C) Copyright 2016 Nexell
 * Hyunseok, Jung <hsjung@nexell.co.kr>
 *
 * SPDX-License-Identifier:      GPL-2.0+
 */

#include <config.h>
#include <common.h>
#ifdef CONFIG_PWM_NX
#include <pwm.h>
#endif
#include <asm/io.h>

#include <asm/arch/nexell.h>
#include <asm/arch/nx_gpio.h>


DECLARE_GLOBAL_DATA_PTR;

/*------------------------------------------------------------------------------
 * intialize nexell soc and board status.
 */

/* call from u-boot */
int board_early_init_f(void)
{
	return 0;
}

void board_gpio_init(void)
{
	nx_gpio_initialize();
	nx_gpio_set_base_address(0, (void *)PHY_BASEADDR_GPIOA);
	nx_gpio_set_base_address(1, (void *)PHY_BASEADDR_GPIOB);
	nx_gpio_set_base_address(2, (void *)PHY_BASEADDR_GPIOC);
	nx_gpio_set_base_address(3, (void *)PHY_BASEADDR_GPIOD);
	nx_gpio_set_base_address(4, (void *)PHY_BASEADDR_GPIOE);
}

#ifdef CONFIG_PWM_NX
void board_backlight_init(void)
{
	pwm_init(CONFIG_BACKLIGHT_CH, CONFIG_BACKLIGHT_DIV,
		 CONFIG_BACKLIGHT_INV);
	pwm_config(CONFIG_BACKLIGHT_CH, TO_DUTY_NS(CONFIG_BACKLIGHT_DUTY,
						   CONFIG_BACKLIGHT_HZ),
		   TO_PERIOD_NS(CONFIG_BACKLIGHT_HZ));
}
#endif

int mmc_get_env_dev(void)
{
	static int envDev = -1;
	int bl1LoadEmmc, ubootLoadPort;

	if( envDev == -1 ) {
		bl1LoadEmmc = readl(PHY_BASEADDR_CLKPWR + SYSRSTCONFIG) >> 19 & 1;
		printf("loaded from %s", bl1LoadEmmc ? "emmc" : "SD");
		ubootLoadPort = readl(SCR_ARM_SECOND_BOOT_REG1);
		switch( ubootLoadPort ) {
		case EMMC_PORT_NUM:
			envDev = 0;
			if( ! bl1LoadEmmc )
				printf("+emmc");
			break;
		case SD_PORT_NUM:
			if( bl1LoadEmmc )
				printf("+SD");
			envDev = 1;
			break;
		default:
			printf("+unknown(%d)", ubootLoadPort);
			envDev = 1;
			break;
		}
		printf(", getting env from MMC %d\n", envDev);
	}
	return envDev;
}

int board_init(void)
{
	board_gpio_init();
#ifdef CONFIG_PWM_NX
	board_backlight_init();
#endif
	return 0;
}

/* u-boot dram initialize  */
int dram_init(void)
{
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
	return 0;
}

/* u-boot dram board specific */
void dram_init_banksize(void)
{
	/* set global data memory */
	gd->bd->bi_arch_number = machine_arch_type;
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x00000100;

	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_SDRAM_SIZE;
}

int board_late_init(void)
{
	return 0;
}

