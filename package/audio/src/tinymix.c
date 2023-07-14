#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <tinyalsa/asoundlib.h>

#include "tinymix.h"

struct control_value
{
	int value;
	int is_percent;
	int is_relative;
};

struct parsed_int
{
	/** Wether or not the integer was valid. */
	int valid;
	/** The value of the parsed integer. */
	int value;
	/** The number of characters that were parsed. */
	unsigned int length;
	/** The number of characters remaining in the string. */
	unsigned int remaining_length;
	/** The remaining characters (or suffix) of the integer. */
	const char* remaining;
};

static struct parsed_int parse_int(const char* str)
{
	struct parsed_int out = {
		0 /* valid */,
		0 /* value */,
		0 /* length */,
		0 /* remaining length */,
		NULL /* remaining characters */
	};

	size_t length = strlen(str);
	size_t i = 0;
	int negative = 0;

	if (i < length && str[i] == '-')
	{
		negative = 1;
		i++;
	}

	while (i < length)
	{
		char c = str[i++];

		if (c < '0' || c > '9')
		{
			--i;
			break;
		}

		out.value *= 10;
		out.value += c - '0';

		out.length++;
	}

	if (negative)
	{
		out.value *= -1;
	}

	out.valid = out.length > 0;
	out.remaining_length = length - out.length;
	out.remaining = str + out.length;

	return out;
}

static struct control_value to_control_value(const char* value_string)
{
	struct parsed_int parsed_int = parse_int(value_string);

	struct control_value out = {
		0 /* value */,
		0 /* is percent */,
		0 /* is relative */
	};

	out.value = parsed_int.value;

	unsigned int i = 0;

	if (parsed_int.remaining[i] == '%')
	{
		out.is_percent = 1;
		i++;
	}

	if (parsed_int.remaining[i] == '+')
	{
		out.is_relative = 1;
	}
	else if (parsed_int.remaining[i] == '-')
	{
		out.is_relative = 1;
		out.value *= -1;
	}

	return out;
}

static int is_int(const char *value)
{
	return value[0] >= '0' && value[0] <= '9';
}

static int isnumber(const char *str)
{
	char * end;

	if (str == NULL || strlen(str) == 0)
		return 0;

	strtol(str, &end, 0);
	return strlen(end) == 0;
}

static void tinymix_set_byte_ctl(struct mixer_ctl * ctl, char ** values, unsigned int num_values)
{
	int ret;
	char * buf;
	char * end;
	unsigned int i;
	long n;

	buf = calloc(1, num_values);
	if (buf == NULL)
	{
		return;
	}

	for (i = 0; i < num_values; i++)
	{
		errno = 0;
		n = strtol(values[i], &end, 0);
		if (*end)
		{
			fprintf(stderr, "%s not an integer\n", values[i]);
			goto fail;
		}
		if (errno)
		{
			fprintf(stderr, "strtol: %s: %s\n", values[i],
				strerror(errno));
			goto fail;
		}
		if (n < 0 || n > 0xff)
		{
			fprintf(stderr, "%s should be between [0, 0xff]\n",
				values[i]);
			goto fail;
		}
		buf[i] = n;
	}

	ret = mixer_ctl_set_array(ctl, buf, num_values);
	if (ret < 0)
	{
		fprintf(stderr, "Failed to set binary control\n");
		goto fail;
	}

	free(buf);
	return;

fail:
	free(buf);
}

static int set_control_value(struct mixer_ctl * ctl, unsigned int i, const struct control_value * value)
{
	int next_value = value->value;

	if (value->is_relative)
	{
		int prev_value = value->is_percent ? mixer_ctl_get_percent(ctl, i) : mixer_ctl_get_value(ctl, i);

		if (prev_value < 0)
		{
			return prev_value;
		}

		next_value += prev_value;
	}

	return value->is_percent ? mixer_ctl_set_percent(ctl, i, next_value) : mixer_ctl_set_value(ctl, i, next_value);
}

static int set_control_values(struct mixer_ctl * ctl, char ** values, unsigned int num_values)
{
	unsigned int num_ctl_values = mixer_ctl_get_num_values(ctl);

	if (num_values == 1)
	{
		/* Set all values the same */
		struct control_value value = to_control_value(values[0]);

		for (unsigned int i = 0; i < num_values; i++)
		{
			int res = set_control_value(ctl, i, &value);
			if (res != 0)
			{
				fprintf(stderr, "Error: invalid value (%d%s%s)\n", value.value, value.is_relative ? "r" : "", value.is_percent ? "%" : "");
				return -1;
			}
		}
	}
	else
	{
		/* Set multiple values */
		if (num_values > num_ctl_values)
		{
			fprintf(stderr, "Error: %u values given, but control only takes %u\n", num_values, num_ctl_values);
			return -1;
		}

		for (unsigned int i = 0; i < num_values; i++)
		{
			struct control_value v = to_control_value(values[i]);

			int res = set_control_value(ctl, i, &v);
			if (res != 0)
			{
				fprintf(stderr, "Error: invalid value (%d%s%s) for index %u\n", v.value, v.is_relative ? "r" : "", v.is_percent ? "%" : "", i);
				return -1;
			}
		}
	}

	return 0;
}

int set_values(struct mixer * mixer, const char * control, char ** values, unsigned int num_values)
{
	struct mixer_ctl * ctl;
	enum mixer_ctl_type type;

	if (isnumber(control))
	{
		ctl = mixer_get_ctl(mixer, atoi(control));
	}
	else
	{
		ctl = mixer_get_ctl_by_name(mixer, control);
	}

	if (ctl == NULL)
	{
		fprintf(stderr, "Invalid mixer control\n");
		return -1;
	}

	type = mixer_ctl_get_type(ctl);

	if (type == MIXER_CTL_TYPE_BYTE)
	{
		tinymix_set_byte_ctl(ctl, values, num_values);
		return 0;
	}

	if (is_int(values[0]))
	{
		set_control_values(ctl, values, num_values);
	}
	else
	{
		if (type == MIXER_CTL_TYPE_ENUM)
		{
			if (num_values != 1)
			{
				fprintf(stderr, "Enclose strings in quotes and try again\n");
				return -1;
			}
			if (mixer_ctl_set_enum_by_string(ctl, values[0]))
			{
				fprintf(stderr, "Error: invalid enum value\n");
				return -1;
			}
		}
		else
		{
			fprintf(stderr, "Error: only enum types can be set with strings\n");
			return -1;
		}
	}

	return 0;
}

int set_value(struct mixer * mixer, const char * control, char * value)
{
	char * values[1];
	values[0] = value;
	return set_values(mixer, control, (char **)values, 1);
}

int get_int_value(struct mixer * mixer, const char * control, int * value, int * min, int * max)
{
	struct mixer_ctl * ctl;
	enum mixer_ctl_type type;

	if (isnumber(control))
	{
		ctl = mixer_get_ctl(mixer, atoi(control));
	}
	else
	{
		ctl = mixer_get_ctl_by_name(mixer, control);
	}

	if (ctl == NULL)
	{
		return 0;
	}

    type = mixer_ctl_get_type(ctl);

	if (type == MIXER_CTL_TYPE_INT)
	{
		*value = mixer_ctl_get_value(ctl, 0);
		*min = mixer_ctl_get_range_min(ctl);
		*max = mixer_ctl_get_range_max(ctl);
		return 1;
	}

	return 0;
}