#include "transfer.h"

/*
 * @brief receive values from uart and send it on CAN bus
 */
void	uart_to_can(t_can_msg *msg, u8 *buffer)
{

	for (u8 i = 0; i < 8; i++)
		buffer[i] = 0x0;
	can_set_msg(msg, 0x0, 0, 0, 0, buffer);
	uart_gets(buffer);
	str_to_can(msg, buffer);
	if (msg->id > 0)
		can_send(msg);
}

/*
 * @brief Parse a string into a valid CAN message
 * @param msg The CAN message to prepare
 * @param str The string formatted like 01234567 0123456789ABCDEF
 *                                      |||||||| ||||||||||||||||
 *                                      IDIDIDID DATADATADATADATA
 *                      ID ________________/   /       /
 *                   space ___________________/       /
 *      up to 8 octet data __________________________/
 */
void	str_to_can(t_can_msg *msg, u8 *str)
{
	u8	id[8 + 1];
	u8	payload[16 + 1];
	u8	tmp[2 + 1];
	u8	i;
	u8	j;

	for (u8 m = 0; m <= 8; m++)
		id[m] = '\0';
	for (u8 m = 0; m <= 16; m++)
		payload[m] = '\0';
	for (u8 m = 0; m <= 2; m++)
		tmp[m] = '\0';

	i = -1;
	while (++i < 8 /*&& ishexa(str[i])*/)
		id[i] = str[i];
	msg->id = atoi_hex(id);
	while (isspace(str[i]))
		i++;
	j = 0;
	while (j < 16 /*&& ishexa(str[i])*/)
	{
		payload[j] = str[i];
		i++;
		j++;
	}
	i = -1;
	for (u8 k = 0; k < 16 ; k++)
	{
		for (u8 l = 0; l < 2; l++)
			tmp[l] = payload[k + l];
		if (!(k % 2))
			msg->data[++i] = atoi_hex(tmp);
	}
	msg->len = i + 1;
}

/*
 * @brief Return an int value from hexadecimal string
 */
u32		atoi_hex(const char *str)
{
	u32	i;
	u32	result;

	i = 0;
	result = 0;
	while (isspace(str[i]))
		i++;
	while (ishexa(str[i]) != '\0')
	{
		result *= 16;
		if (str[i] >= '0' && str[i] <= '9') 
			result += (str[i] - '0');
		else if (str[i] >= 'A' && str[i] <= 'F')
			result += ((str[i] - 'A') + 10);
		else if (str[i] >= 'a' && str[i] <= 'f')
			result += ((str[i] - 'a') + 10);
		i++;
	}
	return (result);
}

/*
 * @brief Return 1 if c is a space character
 */
int		isspace(int c)
{
	if ((c >= 9 && c <= 13) || c == ' ')
		return (1);
	else
		return (0);
}

/*
 * @brief Return 1 if c is a valid hexadecimal character
 */
int		ishexa(int c)
{
	if (  (c >= '0' && c <= '9')
			| (c >= 'A' && c <= 'F')
			| (c >= 'a' && c <= 'f'))
		return (1);
	else
		return (0);
}
/* EOF */
