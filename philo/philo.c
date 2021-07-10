#include "philo.h"

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);
	return (0);
}
int	input_error(char *av[])
{
	int	i;
	int	j;
	int	error;

	i = 1;
	error = 0;
	while (av[i])
	{
		j = 0;
		while (av[i][j])
			if (!ft_isdigit(av[i][j++]))
				error++;
		i++;
	}
	if (ft_atoi(av[1]) <= 1 || error)
	{
		printf("error\n");
		return (1);
	}
	return (0);
}

int	ft_atoi(const char *str)
{
	int i;
	int number;
	int sign;
	int test;

	i = 0;
	number = 0;
	sign = 1;
	test = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			sign = -1;
	while (str[i] >= 48 && str[i] <= 57 && ++test)
		number = (number * 10) + str[i++] - 48;
	if (test > 10 && sign == 1)
		return (-1);
	if (test > 10 && sign == -1)
		return (0);
	return (number * sign);
}


void	init_data(char *av[])
{
	int	i;

	g_philo = malloc(sizeof(t_philo));
	g_philo->philo_num = ft_atoi(av[1]);
	g_philo->forks = malloc(sizeof(pthread_mutex_t) * g_philo->philo_num);
	g_philo->threads = malloc(sizeof(pthread_t) * g_philo->philo_num);
	g_philo->eating = malloc(sizeof(long long) * g_philo->philo_num);
	g_philo->sleeping = malloc(sizeof(long long) * g_philo->philo_num);
	g_philo->thinking = malloc(sizeof(long long) * g_philo->philo_num);
	g_philo->start = malloc(sizeof(long long) * g_philo->philo_num);
	g_philo->time_to_die = ft_atoi(av[2]) * 1000;
	g_philo->time_to_eat = ft_atoi(av[3]) * 1000;
	g_philo->time_to_sleep = ft_atoi(av[4]) * 1000;
	g_philo->num_of_tm_philo_must_eat = 0;
	if (av[5])
	{
		g_philo->num_of_tm_philo_must_eat = ft_atoi(av[5]);
		g_philo->eat = malloc(sizeof(int) * g_philo->philo_num);
	}
	i = 0;
	while (i < g_philo->philo_num)
	{
		pthread_mutex_init(&(g_philo->forks[i]), NULL);
		g_philo->start[i] = 0;
		if (g_philo->eat)
			g_philo->eat[i] = 0;
		g_philo->eating[i] = 0;
		g_philo->thinking[i] = 0;
		g_philo->sleeping[i++] = 0;
	}
	pthread_mutex_init(&g_philo->print, NULL);
}

long long	gettime(void)
{
	struct timeval now;

	gettimeofday(&now, NULL);
	return (now.tv_usec + (now.tv_sec * 1000000));
}


void takefork(int philo, int fork)
{
	pthread_mutex_lock(&g_philo->print);
	printf("%015lld %d has taken a fork\n", gettime() / 1000, philo + 1);
	pthread_mutex_unlock(&g_philo->print);
}

void	eating(int philo)
{
	int	eatingtime;

	g_philo->eating[philo] = gettime();
	pthread_mutex_lock(&g_philo->print);
	printf("%015lld %d is eating\n", gettime() / 1000, philo + 1);
	pthread_mutex_unlock(&g_philo->print);
}

void	sleeping(int philo)
{
	int	sleepingtime;

	g_philo->sleeping[philo] = gettime();
	pthread_mutex_lock(&g_philo->print);
	printf("%015lld %d is sleeping\n", gettime() / 1000, philo + 1);
	pthread_mutex_unlock(&g_philo->print);
}
void	thinking(int philo)
{
	pthread_mutex_lock(&g_philo->print);
	printf("%015lld %d is thinking\n", gettime() / 1000, philo + 1);
	pthread_mutex_unlock(&g_philo->print);
}

void 	print(int philo)
{
	pthread_mutex_lock(&g_philo->print);
	printf("%015lld %d died\n", gettime() / 1000, philo + 1);
}

void	*lunch_ft(void *x)
{
	int		i;

	i = *(int*)x;
	while (1)
	{
		pthread_mutex_lock(&(g_philo->forks[i]));
		takefork(i, i + 1);
		pthread_mutex_lock(&(g_philo->forks[(i + 1) % g_philo->philo_num]));
		takefork(i, (i + 1) % g_philo->philo_num + 1);
		eating(i);
		g_philo->eat[i] += 1;
		while (gettime() - g_philo->eating[i] <= g_philo->time_to_eat);
		pthread_mutex_unlock(&(g_philo->forks[i]));
		pthread_mutex_unlock(&(g_philo->forks[(i + 1) % g_philo->philo_num]));
		sleeping(i);
		while (gettime() - g_philo->sleeping[i] <= g_philo->time_to_sleep);
		thinking(i);
	}
	return (NULL);
}

int		monitor_of_philos(void)
{
	int	i;

	i = 0;
	int count = 0;
	while (1)
	{
		i = 0;
		while (i < g_philo->philo_num)
		{
			if(gettime() - g_philo->eating[i] >= g_philo->time_to_die)
			{
				print(i);
				return (1);
			}
			if (g_philo->eat[i] >= g_philo->num_of_tm_philo_must_eat)
				count++;
			i++;
		}
		if (count == g_philo->philo_num + 1)
			return 1;
	}
	return (1);
}

void		create_thread(void)
{
	int i;

	i = 0;
	while(i < g_philo->philo_num)
	{
		int *x = malloc(sizeof(int));
		*x = i;
		g_philo->eating[i] = gettime();
		pthread_create(&(g_philo->threads[i]), NULL, &lunch_ft, x);
		usleep(100);
		i++;
	}
}


int	main(int ac, char *av[])
{
	int i;

	if (ac == 6 || ac == 5)
	{
		if (input_error(av))
			return (1);
		init_data(av);
		create_thread();
		return (monitor_of_philos());
	}
	else
		printf("error\n");
	return (1);
}