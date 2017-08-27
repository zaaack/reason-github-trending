type repo = {
  name: string,
  star_total: int,
  fork_total: int,
  star_recent: int,
  desc: string,
  language: string
};

type repos = list repo;

type error = {
  status: int,
  message: string,
  stack: option string
};
