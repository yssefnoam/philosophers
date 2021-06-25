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
	int	mtx;

	g_philo = malloc(sizeof(t_philo));
	g_philo->philo_num = ft_atoi(av[1]);
	g_philo->forks = malloc(sizeof(pthread_mutex_t) * g_philo->philo_num);
	g_philo->threads = malloc(sizeof(pthread_t) * g_philo->philo_num);
	g_philo->time_to_die = ft_atoi(av[2]);
	g_philo->time_to_eat = ft_atoi(av[3]);
	g_philo->time_to_sleep = ft_atoi(av[4]);
	g_philo->num_of_tm_philo_must_eat = 0;
	if (av[5])
		g_philo->num_of_tm_philo_must_eat = ft_atoi(av[5]);
	mtx = 0;
	while (mtx < g_philo->philo_num)
		pthread_mutex_init(&(g_philo->forks[mtx++]), NULL);
	pthread_mutex_init(&(g_philo->lunch_thread), NULL);
	g_philo->time = 0;
}
void take_fork_print(int i)
{
	struct timeval tm;

	gettimeofday(&tm, NULL);
	if (g_philo->time == 0)
	{
		g_philo->time = tm.tv_usec;
		printf("%ld %d has taken a fork\n", tm.tv_usec - tm.tv_usec, i + 1);
		return ;
	}
	printf("%ld %d has taken a fork\n", tm.tv_usec - g_philo->time, i + 1);
}

void	*lunch_ft(void *x)
{
	int i;

	i = *(int*)x;
	pthread_mutex_lock(&(g_philo->lunch_thread));
	pthread_mutex_lock(&(g_philo->forks[i]));
	take_fork_print(i);
	pthread_mutex_lock(&(g_philo->forks[i]));

	return (NULL);
}

void		join_thread(void)
{
	int i;
	usleep(10);

	i = 0;
	while(i < g_philo->philo_num)
	{
		pthread_join(g_philo->threads[i], NULL);
		i++;
	}
}
void		create_thread(void)
{
	int i;

	i = 0;
	while(i < g_philo->philo_num)
	{
		int *x = malloc(sizeof(int));
		*x = i;
		pthread_create(&(g_philo->threads[i]), NULL, &lunch_ft, &x);
		i++;
	}
}

int	main(int ac, char *av[])
{
	if (ac == 6 || ac == 5)
	{
		if (input_error(av))
			return (1);
		init_data(av);
		create_thread();
		// join_thread();
		while(1);
	}
	else
		printf("error\n");
	return (1);
}